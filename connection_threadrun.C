#include "connection_info.H"
#include "connection_thread.H"
#include "returned_pointer.H"

#include <x/sysexception.H>

LIBCXXW_NAMESPACE_START

// Figure out what the connection thread needs to do next. It could be:
//
// 1. Dispatch a message from its message queue.
//
// 2. An X event, received via xcb_poll_event.
//
// 3.
void connectionObj::implObj::threadObj
::run_something(msgqueue_auto &msgqueue,
		struct pollfd *topoll,
		size_t &npoll)
{
	// See if there's a message to be dispatched on the message queue.

	if (!msgqueue->empty())
	{
		msgqueue.event();
		return;
	}

	// Check if the connection errored out, if not, check for
	// a message.

	if (npoll == 2 && xcb_connection_has_error(info->conn))
	{
		LOG_FATAL("Connection to the X server has a fatal error");
		npoll=1;
		topoll[1].revents=0;
	}

	if (npoll == 2)
	{
		auto event=return_pointer(xcb_poll_for_event(info->conn));

		if (event)
		{
			LOG_DEBUG("Processing event "
				  << (int)(event->response_type & ~0x80)
				  << (event->response_type & 0x80
				      ? " (SendEvent)":""));

			run_event(event);
			return;
		}

		// Flush anything we have.
		xcb_flush(info->conn);
	}

	// Ok, nothing else to do but poll().

	if (poll(topoll, npoll, -1) < 0)
	{
		if (errno != EINTR && errno != EAGAIN &&
		    errno != EWOULDBLOCK)
		{
			LOG_FATAL("poll() failed");
			throw SYSEXCEPTION("poll");
		}
	}
}

LIBCXXW_NAMESPACE_END
