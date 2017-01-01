/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include "connection_thread.H"
#include "element.H"
#include "returned_pointer.H"
#include "catch_exceptions.H"
#include <x/sysexception.H>

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
	connection_thread me(this);

	LOG_FUNC_SCOPE(runLogger);

	// Process all messages on the queue, this takes priority.

	while (!msgqueue->empty())
		msgqueue.event();

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
		bool processed_messages=false;

		while (auto event=return_pointer(xcb_poll_for_event(info->conn)))
		{
			processed_messages=true;

			LOG_TRACE("Processing event "
				  << (int)(event->response_type & ~0x80)
				  << (event->response_type & 0x80
				      ? " (SendEvent)":""));

			run_event(event);
		}

		if (processed_messages)
			return;

		// Flush anything we have.
		xcb_flush(info->conn);
	}

	if (!visibility_updated_thread_only->empty())
	{
		// Some display element changed their visibility. Invoke
		// their update_visibility() methods.
		while (!visibility_updated_thread_only->empty())
		{
			auto iter=visibility_updated_thread_only->begin();

			auto element=*iter;

			visibility_updated_thread_only->erase(iter);

			element->update_visibility(me);
		}
		return;
	}

	// Ok, no more work to do, and we were asked to politely stop.
	if (stop_received)
	{
		stopping_politely=true;
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
	LOG_TRACE("Dispatching: " << file << "(" << line << ")");

	func(connection_thread(this));
}


LIBCXXW_NAMESPACE_END
