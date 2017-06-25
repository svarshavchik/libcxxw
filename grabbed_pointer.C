/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "grabbed_pointer.H"
#include "element.H"
#include "connection_thread.H"
#include "connection.H"
#include "returned_pointer.H"
#include "messages.H"
#include "x/w/screen.H"
#include <x/exception.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::grabbed_pointerObj);

LIBCXXW_NAMESPACE_START

static xcb_grab_status_t do_grab(xcb_connection_t *c,
				 uint8_t           owner_events,
				 xcb_window_t      grab_window,
				 uint8_t           pointer_mode,
				 uint8_t           keyboard_mode,
				 xcb_window_t      confine_to,
				 xcb_cursor_t      cursor,
				 xcb_timestamp_t   time)
{
	const auto &logger=grabbed_pointerObj::logger;

	returned_pointer<xcb_generic_error_t *> error;

	auto value=return_pointer
		(xcb_grab_pointer_reply
		 (c,
		  xcb_grab_pointer(c, owner_events, grab_window,
				   XCB_EVENT_MASK_BUTTON_PRESS |
				   XCB_EVENT_MASK_BUTTON_RELEASE |
#if 0
				   XCB_EVENT_MASK_ENTER_WINDOW |
				   XCB_EVENT_MASK_LEAVE_WINDOW |
#endif
				   XCB_EVENT_MASK_POINTER_MOTION,
				   pointer_mode,
				   keyboard_mode,
				   confine_to,
				   cursor,
				   time),
		  error.addressof()));

	if (error)
	{
		LOG_ERROR(connection_error(error));
		return XCB_GRAB_STATUS_INVALID_TIME;
	}

	return (xcb_grab_status_t)value->status;
}

grabbed_pointerObj::grabbed_pointerObj(IN_THREAD_ONLY,
				       const ref<elementObj::implObj>
				       &grabbing_element)
	: grabbing_element{grabbing_element},
	  grabbed_window(&grabbing_element->get_window_handler()),
	  timestamp{IN_THREAD->timestamp(IN_THREAD)},
	  result(do_grab(conn()->conn, false,
			 grabbed_window->id(),
			 XCB_GRAB_MODE_SYNC,
			 XCB_GRAB_MODE_ASYNC,
			 XCB_NONE,
			 XCB_NONE, timestamp))
{
	switch (result) {
	case XCB_GRAB_STATUS_SUCCESS:
		break;
	case XCB_GRAB_STATUS_ALREADY_GRABBED:
		LOG_ERROR(_("Attempt to grab a pointer failed because it's already grabbed"));
	case XCB_GRAB_STATUS_INVALID_TIME:
		break;
	case XCB_GRAB_STATUS_NOT_VIEWABLE:
		LOG_ERROR(_("Attempt to grab a pointer in a non-viewable window has failed"));
	case XCB_GRAB_STATUS_FROZEN:
		throw EXCEPTION(_("Attempt to grab a pointer while input processing is frozen"));
	default:
		std::ostringstream o;

		o << result;

		LOG_ERROR(_("Attempt to grab a pointer failed, error code: ") << o.str());
	}
}

grabbed_pointerObj::~grabbed_pointerObj()
{
	if (result == XCB_GRAB_STATUS_SUCCESS)
		xcb_ungrab_pointer(conn()->conn, timestamp);
}

connection_info grabbed_pointerObj::conn() const
{
	return grabbed_window->screenref->get_connection()->impl->info;
}

bool grabbed_pointerObj::succeeded() const
{
	return result == XCB_GRAB_STATUS_SUCCESS;
}

void grabbed_pointerObj::allow_events()
{
	if (result == XCB_GRAB_STATUS_SUCCESS)
		xcb_allow_events(conn()->conn, XCB_ALLOW_ASYNC_POINTER,
				 timestamp);
}

///////////////////////////////////////////////////////////////////////////

grabbed_pointerptr elementObj::implObj::grab_pointer(IN_THREAD_ONLY)
{
	if ( !data(IN_THREAD).inherited_visibility ||
	     data(IN_THREAD).removed)
		return grabbed_pointerptr();

	auto &window_grab=
		get_window_handler().current_pointer_grab(IN_THREAD);

	auto p=window_grab.getptr();

	if (p)
		return grabbed_pointerptr(); // Another element grabbed.

	auto gp=grabbed_pointer::create(IN_THREAD, ref<implObj>(this));

	if (!gp->succeeded())
		return grabbed_pointerptr();

	window_grab=gp;

	return gp;
}

LIBCXXW_NAMESPACE_END
