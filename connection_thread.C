/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_thread.H"
#include "window_handler.H"
#include "x/w/impl/element.H"
#include "x/w/impl/container.H"
#include "batch_queue.H"
#include "xid_t.H"
#include "catch_exceptions.H"
#include "selection/incremental_selection_updates.H"
#include <x/functionalrefptr.H>
#include <x/mcguffinmultimap.H>
#include <x/sentry.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::connection_threadObj);

LIBCXXW_NAMESPACE_START

connection_threadObj
::connection_threadObj(const connection_info &info)
	: info{info},
	  disconnect_callback_thread_only{[] {}}
{
}

connection_threadObj::~connection_threadObj()=default;

void connection_threadObj::run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin)
{
	msgqueue_auto msgqueue(this);
	msgqueue_auto msgbatched_queue(this, batched_queue);

	threadmsgdispatcher_mcguffin=nullptr;

	LOG_DEBUG("Connection thread started");

	// Initialize thread-only variables

	std::unordered_map<xcb_window_t,
			   ref<window_handlerObj>> window_handlers;
	std::unordered_map<uint32_t, ref<xidObj>> destroyed_xids;

	element_set_t visibility_updated;
	elements_to_redraw_set elements_to_redraw;
	containers_2_recalculate_map containers_2_recalculate;
	containers_2_batch_recalculate_set containers_2_batch_recalculate;
	elements_2_batch_showhide_map elements_2_batch_showhide;

	element_set_t element_position_updated;
	scheduled_callbacks_t scheduled_callbacks=
		scheduled_callbacks_t::create();
	incremental_selection_update_info pending_incremental_updates;
	idle_callbacks_t idle_callbacks;

	functionref<void (ONLY IN_THREAD)> cxxwtheme_changed=
		[](ONLY IN_THREAD) {};

	window_handlers_thread_only= &window_handlers;
	destroyed_xids_thread_only= &destroyed_xids;
	elements_to_redraw_thread_only= &elements_to_redraw;
	containers_2_recalculate_thread_only= &containers_2_recalculate;
	containers_2_batch_recalculate_thread_only=
		&containers_2_batch_recalculate;
	elements_2_batch_showhide_thread_only=
		&elements_2_batch_showhide;
	element_position_updated_thread_only= &element_position_updated;
	scheduled_callbacks_thread_only= &scheduled_callbacks;
	pending_incremental_updates_thread_only= &pending_incremental_updates;
	idle_callbacks_thread_only= &idle_callbacks;

	visibility_updated_thread_only= &visibility_updated;

	root_window_thread_only=XCB_NONE;
	cxxwtheme_changed_thread_only=&cxxwtheme_changed;

	// Set two file descriptors to poll.
	//
	// 1. The message queue event file descriptor, set it to
	//    non-blocking mode.
	//
	// 2. xcb_connection_t file descriptor.
	//
	// n_poll also gets initialized to 2, indicating 2 file descriptors
	// will be polled. But before we poll, if xcb_connection_has_error(),
	// n_poll gets decremented to 1, and we'll only handle event messages,
	// until doom arrives.

	auto eventfd=msgqueue->get_eventfd();
	eventfd->nonblock(true);

	struct pollfd pfd[2];

	pfd[0].fd=eventfd->get_fd();
	pfd[0].events=POLLIN | POLLHUP;

	pfd[1].fd=xcb_get_file_descriptor(info->conn);
	pfd[1].events=POLLIN | POLLHUP;

	size_t n_poll=2;

	stop_received=false;
	disconnected_flag_thread_only=false;

	do
	{
		try {
			int poll_for;

			if (!run_something(msgqueue, pfd, n_poll, poll_for))
				// The connection thread needs to poll() ONLY
				// if run_something() returned at the very
				// end. All intermediate returns from
				// run_something() mean: try again.
				continue;

			if (stop_received)
				// Ok, no more work to do, and we were asked
				// to politely stop.
				continue;

			if (poll(pfd, n_poll, poll_for) < 0)
			{
				if (errno != EINTR && errno != EAGAIN &&
				    errno != EWOULDBLOCK)
				{
					LOG_FATAL("poll() failed");
					continue;
				}
			}

			if (pfd[0].revents & POLLIN)
				msgqueue->get_eventfd()->event();
		} CATCH_EXCEPTIONS;
	} while (!stop_received);
}

void connection_threadObj::report_error(const xcb_generic_error_t *e)
{
	LOG_ERROR(connection_error(e));
}

batch_queue connection_threadObj::get_batch_queue()
{
	mpobj<weakptr<batch_queueptr>>::lock lock(current_batch_queue);

	// Return the existing batch_queue object, if there is one.

	auto p=lock->getptr();

	if (!p.null())
		return p;

	auto new_batch_queue=batch_queue::create(connection_thread(this));

	*lock=new_batch_queue;

	return new_batch_queue;
}

bool connection_threadObj::process_buffered_events(ONLY IN_THREAD)
{
	bool processed_buffered_event=false;

	for (const auto &window_handler:*window_handlers(IN_THREAD))
	{
		auto &w=*window_handler.second;

		// We get ConfigureNotify, followed by Exposure events, which
		// we buffer up.
		//
		// Now, it's time to pay the piper, and dispatch these events
		// to the windows, and we do ConfigureNotify first, then
		// Exposure, in the same order.

		if (w.pending_configure_notify_event(IN_THREAD))
		{
			// One or more ConfigureNotify events were received.
			w.pending_configure_notify_event(IN_THREAD)=false;

			try {
				w.process_configure_notify(IN_THREAD);
			} CATCH_EXCEPTIONS;

			processed_buffered_event=true;
		}
	}

	// If we received and process any ConfigureNotifys we bail out
	// now. This results in the following sequence of events.
	//
	// 1. We get a ConfigureNotify.
	// 2. generic_windowObj::handlerObj::process_configure_notify arranges
	//    things so that (hopefully), exposure events get received and
	//    queued up.
	// 3. Configure processing resizes all events. After we return from
	//    here we're going to recalculate all the containers and move
	//    everything that needs to be moved.
	// 4. Finally everything's been recalculated and repositioned, and
	//    we wind up back there, so we can look at the exposure events,
	//    in Part II, and perform exposure processing, drawing everything
	//    where it's been moved to.

	if (processed_buffered_event)
		return true;

	for (const auto &window_handler:*window_handlers(IN_THREAD))
	{
		auto &w=*window_handler.second;

		auto &exposure_rectangles=w.exposure_rectangles(IN_THREAD);

		// If we are expecting a full_exposure, we'll do exposure
		// processing even if the rectangle set is empty.
		//
		// But if we received some exposure rectangles, then we
		// will wait for exposure processing until
		// exposure_rectangles.complete, even if we're expecting
		// to do a full_exposure.

		bool process_exposure=false;

		if (exposure_rectangles.rectangles.empty())
		{
			process_exposure=exposure_rectangles.full_exposure;
		}
		else
		{
			process_exposure=exposure_rectangles.complete;
		}

		if (process_exposure)
		{
			// Exposure events were received, and the last one
			// had a 0 count.

			try {
				w.process_collected_exposures(IN_THREAD);
			} CATCH_EXCEPTIONS;

			exposure_rectangles.rectangles.clear();
			processed_buffered_event=true;
		}

		// Same logic for graphics exposures

		auto &graphics_exposure_rectangles=
			w.exposure_rectangles(IN_THREAD);

		if (!graphics_exposure_rectangles.rectangles.empty() &&
		    graphics_exposure_rectangles.complete)
		{
			try {
				w.process_collected_graphics_exposures
					(IN_THREAD);
			} CATCH_EXCEPTIONS;

			graphics_exposure_rectangles.rectangles.clear();
			processed_buffered_event=true;
		}
	}

	return processed_buffered_event;
}

void connection_threadObj::release_grabs(ONLY IN_THREAD)
{
	for (const auto &window_handler:*window_handlers(IN_THREAD))
	{
		auto &w=*window_handler.second;

		// At this point, all window activity has ceased, so any
		// grabs can be released.
		w.release_grabs(IN_THREAD);
	}
}

void connection_threadObj
::install_window_handler(ONLY IN_THREAD,
			 const ref<window_handlerObj> &handler)
{
	IN_THREAD->window_handlers(IN_THREAD)
		->insert({handler->id(), handler});
	handler->installed(IN_THREAD);

	if (disconnected_flag_thread_only) // Already disconnected
		try {
			handler->disconnected(IN_THREAD);
		} CATCH_EXCEPTIONS;
}

void connection_threadObj
::uninstall_window_handler(ONLY IN_THREAD,
			   const ref<window_handlerObj> &handler)
{
	auto window_id=handler->id();

	//! If it's grabbed something, ungrab it.
	handler->ungrab(IN_THREAD);
	window_handlers(IN_THREAD)->erase(window_id);
	destroyed_xids(IN_THREAD)->insert({window_id,handler->xid_obj});
}

LIBCXXW_NAMESPACE_END
