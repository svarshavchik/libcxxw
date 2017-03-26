/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "returned_pointer.H"
#include "catch_exceptions.H"
#include "draw_info_cache.H"
#include <x/sysexception.H>
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

void connection_threadObj::dispatch_install_on_disconnect(const std::function<void ()> &callback)
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

void connection_threadObj
::run_something(msgqueue_auto &msgqueue,
		struct pollfd *topoll,
		size_t &npoll)
{
	connection_thread thread_(this);

	LOG_FUNC_SCOPE(runLogger);

	// Construct a draw_info_cache that will persistent throughout all
	// callbacks.
	draw_info_cache current_draw_info_cache;
	current_draw_info_cache_thread_only= &current_draw_info_cache;

	while (1)
	{
		if (recalculate_containers(IN_THREAD))
			continue;

		if (process_element_position_updated(IN_THREAD))
			continue;

		if (process_visibility_updated(IN_THREAD))
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

			if (!current_draw_info_cache.draw_info_cache.empty())
				// Don't go back to the top of the loop,
				// above, and potentially execute something
				// that invalidates the cached draw_info
				// data. Rather return, and go back here,
				// and create a new draw_info_cache.
				return;

			continue;
		}

		// Check if the connection errored out, if not, check for
		// a message.

		if (npoll == 2 && xcb_connection_has_error(info->conn))
		{
			LOG_FATAL("Connection to the X server has a fatal error");
			npoll=1;
			topoll[1].revents=0;
			try {
				disconnect_callback_thread_only();
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
		if (redraw_elements(IN_THREAD))
			// Don't bother checking draw_info_cache. It's unlikely
			// that redraw_elements() did anything that might
			// generate recalculation or processing activity,
			// above. To be on the save side, return and go back
			// here with a freshly wiped draw_info_cache, and
			// take it from the top.
			return;
		break;
	}

	xcb_flush(info->conn);

	// Ok, no more work to do, and we were asked to politely stop.
	if (stop_received)
	{
		stopping_politely=true;
		return;
	}
	// Ok, nothing else to do but poll().
	LOG_TRACE("Polling");
	if (poll(topoll, npoll, -1) < 0)
	{
		if (errno != EINTR && errno != EAGAIN &&
		    errno != EWOULDBLOCK)
		{
			LOG_FATAL("poll() failed");
			throw SYSEXCEPTION("poll");
		}
	}

	if (topoll[0].revents & POLLIN)
		msgqueue->getEventfd()->event();
}

void connection_threadObj
::dispatch_do_run_as(const char *file,
		     int line,
		     const x::function<void (IN_THREAD_ONLY)> &func)
{
	LOG_FUNC_SCOPE(runLogger);
	LOG_DEBUG("Dispatching: " << file << "(" << line << ")");

	// Make sure all changes in the main execution thread are
	// committed by now. Although this should theoretically
	// taken care of by the mutex, this is technically required
	// for the connection thread to see what it needs to see.

	std::atomic_thread_fence(std::memory_order_acquire);

	func(connection_thread(this));
}

LIBCXXW_NAMESPACE_END
