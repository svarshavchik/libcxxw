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

	// Assume we'll poll() indefinitely, unless there's a change in plans.
	int poll_for;

	while (1)
	{
		poll_for= -1;

		if (!current_draw_info_cache.draw_info_cache.empty())
			// Something must've used the draw cache.
			// Don't potentially execute something again
			// that might use the stale cached draw_info
			// data. It might be rendered moot by
			// recalculate_containers() (and some containers'
			// recalculation involves explicit redraws). Rather,
			// return, and go back here, to create a new
			// draw_info_cache.
				return;

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
			continue;
		}

		// Search: are there any scheduled callbacks?

		if (invoke_scheduled_callbacks(IN_THREAD, poll_for))
			continue;

		expire_incremental_updates(IN_THREAD, poll_for);

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
				if (!current_draw_info_cache.draw_info_cache.empty())
					return; // Look above.
				continue;
			}

			if (process_buffered_motion_event(IN_THREAD))
				continue;
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

	allow_events(IN_THREAD);
	xcb_flush(info->conn);

	// Ok, no more work to do, and we were asked to politely stop.
	if (stop_received)
	{
		stopping_politely=true;
		return;
	}
	// Ok, nothing else to do but poll().
	LOG_TRACE("Polling");
	if (poll(topoll, npoll, poll_for) < 0)
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

// Insert a new callback

ref<obj> connection_threadObj
::do_schedule_callback(IN_THREAD_ONLY,
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

bool connection_threadObj::invoke_scheduled_callbacks(IN_THREAD_ONLY,
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
				p->invoke(IN_THREAD);
			} CATCH_EXCEPTIONS;
			invoked=true;
			p=nullptr;

			// At this point, if the iterator hasn't reached
			// end, b->first must be greater than now.
		}

		if (b != e)
			poll_for=compute_poll_until(now, b->first);

		break;
	}

	return invoked;
}

int connection_threadObj::compute_poll_until(tick_clock_t::time_point now,
					     tick_clock_t::time_point when)
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
		return 30*60 * 1000;

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
	return v;
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
