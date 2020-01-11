/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "returned_pointer.H"
#include "catch_exceptions.H"
#include "window_handler.H"
#include "batch_queue.H"
#include <x/sysexception.H>
#include <x/functionalrefptr.H>
#include <x/mcguffinmultimap.H>
#include <atomic>

LIBCXXW_NAMESPACE_START

// Received a message to stop, politely.

void connection_threadObj::stop()
{
	stop_politely();
}

void connection_threadObj::dispatch_stop_politely()
{
	stop_received=true;
	LOG_DEBUG("Connection thread stop message received");
}

void connection_threadObj::dispatch_install_on_disconnect(const functionref
							  <void ()> &callback)
{
	disconnect_callback_thread_only=callback;
}

// Figure out what the connection thread needs to do next. It could be:
//
// 1. Dispatch a message from its message queue.
//
// 2. An X event, received via xcb_poll_event.
//
// 3.
LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::connection_threadObj::run_something,
		    runLogger);

bool connection_threadObj
::run_something(msgqueue_auto &msgqueue,
		struct pollfd *topoll,
		size_t &npoll,
		int &poll_for)
{
	connection_thread IN_THREAD{this};

	LOG_FUNC_SCOPE(runLogger);

	// Assume we'll poll() indefinitely, unless there's a change in plans.

	for ( ; ; )
	{
		poll_for= -1;

		// If there's a batch queue object, it will block all
		// further processing.

		auto b=current_batch_queue.get().getptr();

		if (b)
		{
			while (!msgqueue->empty())
				msgqueue.event();
			// Drained the message queue, wait for an event,
			// there will be at least one from the batch queue's
			// destructor.
			return true;
		}

		if (recalculate_containers(IN_THREAD, poll_for))
			continue;

		if (process_element_position_updated(IN_THREAD,
						     poll_for))
			continue;

		if (process_visibility_updated(IN_THREAD, poll_for))
			continue;

		// Process a message in the message queue. If processing a
		// message resulted in container recalculation, element
		// position update, or visibility update, that was an
		// app request which must be processed before the next message,
		// another app request, gets processed, so we go back to the top
		// before processing the next message.

		if (!msgqueue->empty())
		{
			msgqueue.event();
			continue;
		}

		// Search: are there any scheduled callbacks?

		if (invoke_scheduled_callbacks(IN_THREAD, poll_for))
			continue;

		expire_incremental_updates(IN_THREAD, poll_for);

		bool processed_focus_updates=false;

		for (const auto &handler:*window_handlers_thread_only)
		{
			try {
				handler.second->timeout_selection_request
					(IN_THREAD, poll_for);
			} CATCH_EXCEPTIONS;

			try {
				if (handler.second->process_focus_updates
				    (IN_THREAD))
					processed_focus_updates=true;
			} CATCH_EXCEPTIONS;
		}

		if (processed_focus_updates)
			continue;

		// Check if the connection errored out, if not, check for
		// a message.

		if (npoll == 2 && xcb_connection_has_error(info->conn))
		{
			LOG_FATAL("Connection to the X server has a fatal error");
			npoll=1;
			topoll[1].revents=0;
			disconnected_flag_thread_only=true;
			try {
				disconnect_callback_thread_only();
			} CATCH_EXCEPTIONS;

			for (const auto &handler:*window_handlers_thread_only)
				try {
					handler.second->disconnected(IN_THREAD);
				} CATCH_EXCEPTIONS;
		}

		if (npoll == 2)
		{
			if (auto event=return_pointer(xcb_poll_for_event
						      (info->conn)))
			{
				LOG_TRACE("Processing event "
					  << (int)(event->response_type & ~0x80)
					  << (event->response_type & 0x80
					      ? " (SendEvent)":""));

				run_event(IN_THREAD, event);
				continue;
			}
		}

		if (process_buffered_events(IN_THREAD))
			continue;

		if (redraw_elements(IN_THREAD, poll_for))
			continue;
		break;
	}

	release_grabs(IN_THREAD);

	// Close your eyes, and pretend that this is xcb_flush(). By itself,
	// xcb_flush() is insufficient. While dragging and quickly redrawing
	// the dragged widgets, all the rendering stuff can pile up in the
	// various network buffers (when the client and server are connected
	// over the network) while the client is quickly consuming all the
	// motion events. process_buffered_motion_event() doesn't help us.
	// We seem to be capable of receiving a motion event, and without
	// anything else being received we'll process the motion event, drag
	// all widgets, and push out all the rendering messages, get the
	// next motion event from the server, lather, rinse, repeat. The
	// server can send us motion events faster than it can rerender
	// everything.
	//
	// So what we do is send the InternAtom request and wait for the
	// reply. We do this after we shoved a pile of rendering requests
	// first. After the server chews on them, it acks the InternAtom
	// and we wait until we get it. In the mean time, all MotionNotifys
	// get buffered, we go through them before draining the input queue,
	// and process_buffered_motion_event() deals with the last one:

	(void)info->get_atom("ATOM", false);

	if (npoll == 2)
	{
		// It's possible that our pseudo-flush has buffered some
		// events.

		if (auto event=return_pointer(xcb_poll_for_queued_event
					      (info->conn)))
		{
			LOG_TRACE("Processing event "
				  << (int)(event->response_type & ~0x80)
				  << (event->response_type & 0x80
				      ? " (SendEvent)":""));

			run_event(IN_THREAD, event);
			return false;
		}

		// We check for buffered motion events, and if so, process them
		// at this stage. Dragging a scrollbar that scrolls a peephole
		// may result in large number of drawing operations, so we
		// do this after we redraw, and flush, the display.

		if (process_buffered_motion_event(IN_THREAD))
			return false;
	}

	if (run_idle(IN_THREAD))
		return false;
	return true;
}

// Insert a new callback

ref<obj> connection_threadObj
::do_schedule_callback(ONLY IN_THREAD,
		       const tick_clock_t::duration &timeout,
		       const callback_functional_t &callback)
{
	return (*scheduled_callbacks(IN_THREAD))->insert(tick_clock_t::now()
							 + timeout,
							 callback);
}

// Check scheduled_callbacks() list.

// Returns a bool indicating whether we executed something.
// If false is returned, may update poll_for with the timeout for poll(),
// at which point something might be scheduled, then. Otherwise poll_for
// is left alone (it should be initialized to -1.

bool connection_threadObj::invoke_scheduled_callbacks(ONLY IN_THREAD,
						      int &poll_for)
{
	bool invoked=false;

	// Scan the scheduled_callbacks list.

	auto b=(*scheduled_callbacks(IN_THREAD))->begin();
	auto e=(*scheduled_callbacks(IN_THREAD))->end();

	while (b != e)
	{
		auto p=b->second.getptr();

		if (p.null())
		{
			++b;
			continue; // Zombie callback.
		}

		// At this point we should check the current
		// system clock, and see if the callback time
		// has elapsed.

		auto now=tick_clock_t::now();

		// And take a closer look
		for ( ; b != e && now >= b->first; ++b)
		{
			if (p.null())
				// Second, and subsequent, time here.
				p=b->second.getptr();

			if (p.null())
				continue; // Ignore zombies

			// Remove this callback, and invoke it.
			b->second.erase();

			try {
				p(IN_THREAD);
			} CATCH_EXCEPTIONS;
			invoked=true;
			p=nullptr;

			// At this point, if the iterator hasn't reached
			// end, b->first must be greater than now.
		}

		if (b != e)
			compute_poll_until(now, b->first, poll_for);

		break;
	}

	return invoked;
}

void connection_threadObj::compute_poll_until(tick_clock_t::time_point now,
					      tick_clock_t::time_point when,
					      int &poll_for)
{
	const auto maximum_poll=
		std::chrono::duration_cast<tick_clock_t::duration>
		(std::chrono::minutes(30));

	// We don't have anything more than
	// a few seconds in the future, but
	// just in case we have long terrm
	// plans, limit poll() to 30 minute
	// timeouts.

	if (when > now + maximum_poll)
	{
		const int cantbe=30*60 * 1000;

		if (poll_for < 0 || poll_for > cantbe)
			poll_for=cantbe;
		return;
	}

	auto v=std::chrono::duration_cast
		<std::chrono::milliseconds>
		(when-now).count();

	// It's possible if the system
	// clock's precision is greater
	// than a millisecond, the
	// duration cast produces a
	// goose egg. poll() for a lone
	// millisecond, in that case.

	if (v == 0)
		v=1;

	if (poll_for < 0 || v < poll_for)
		poll_for=v;
}

void connection_threadObj
::dispatch_do_run_as(const x::function<void (ONLY IN_THREAD)> &func)
{
	// Make sure all changes in the main execution thread are
	// committed by now. Although this should theoretically
	// taken care of by the mutex, this is technically required
	// for the connection thread to see what it needs to see.

	std::atomic_thread_fence(std::memory_order_acquire);

	func(connection_thread(this));
}

bool connection_threadObj::run_idle(ONLY IN_THREAD)
{
	if (idle_callbacks(IN_THREAD)->empty())
		return false;

	auto f=idle_callbacks(IN_THREAD)->front();
	idle_callbacks(IN_THREAD)->pop_front();

	try {
		f(IN_THREAD);
	} CATCH_EXCEPTIONS;
	return true;
}

LIBCXXW_NAMESPACE_END
