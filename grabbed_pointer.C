/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "grabbed_pointer.H"
#include "x/w/impl/element.H"
#include "connection_thread.H"
#include "connection.H"
#include "returned_pointer.H"
#include "messages.H"
#include "x/w/screen.H"
#include "x/w/elementobj.H"
#include <x/exception.H>
#include <x/logger.H>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN element_grabObj;

grabbed_pointerObj::grabbed_pointerObj()=default;

grabbed_pointerObj::~grabbed_pointerObj()=default;

//! Implemenent a real pointer grab.

//! Implement the real grabbed_pointerObj.

class LIBCXX_HIDDEN real_pointer_grabObj : public grabbed_pointerObj {

 public:
	//! The logger
	LOG_CLASS_SCOPE;

	//! Which element grabbed me.
	weakptr<ptr<element_grabObj>> grabbing_element;

	//! Which top level window the grab is effective for.
	const ref<generic_windowObj::handlerObj> grabbed_window;

	//! Grabbed timestamp
	const xcb_timestamp_t timestamp;

	//! Result of the grab.
	const xcb_grab_status_t result;

	//! Constructor
	real_pointer_grabObj(ONLY IN_THREAD,
			   const ref<generic_windowObj::handlerObj>
			   &grabbed_window);

private:

	connection_info conn() const;

	bool events_allowed=false;

 public:
	//! Destructor
	~real_pointer_grabObj();

	//! Whether the grab succeeded

	bool succeeded(ONLY IN_THREAD) const override;
	//! Allow events

	void allow_events(ONLY IN_THREAD) override;

	element_implptr get_grab_element(ONLY IN_THREAD) override;

	grabbed_pointerptr create_another_grab(ONLY IN_THREAD,
					       const element_implptr &)
		override;
};

LIBCXXW_NAMESPACE_END

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::real_pointer_grabObj);

LIBCXXW_NAMESPACE_START

//! A subclass of grabbed_pointerObj representing a grab by a specific element.

class element_grabObj : public grabbed_pointerObj {

 public:

	//! The real pointer grab mcguffin.
	const grabbed_pointer real_grab_pointer;

	//! Which element effected the grab.

	//! This element is expected to own this mcguffin.
	weakptr<element_implptr> grabbing_element;

	//! Constructor
	element_grabObj(const grabbed_pointer &real_grab_pointer,
			const element_impl &grabbing_element)
		: real_grab_pointer(real_grab_pointer),
		grabbing_element(grabbing_element)
		{
		}

	//! Return the grabbing element.
	element_implptr get_grab_element(ONLY IN_THREAD) override
	{
		return grabbing_element.getptr();
	}

	//! Forward succeeded() to the real grab mcguffin.
	bool succeeded(ONLY IN_THREAD) const override
	{
		return real_grab_pointer->succeeded(IN_THREAD);
	}

	//! Forward allow_events() to the real grab mcguffin.
	void allow_events(ONLY IN_THREAD) override
	{
		return real_grab_pointer->allow_events(IN_THREAD);
	}

	//! Forward create_another_grab() to the real grab mcguffin.

	grabbed_pointerptr create_another_grab(ONLY IN_THREAD,
					       const element_implptr &e)
		override
	{
		return real_grab_pointer->create_another_grab(IN_THREAD, e);
	}
};


static xcb_grab_status_t do_grab(xcb_connection_t *c,
				 uint8_t           owner_events,
				 xcb_window_t      grab_window,
				 uint8_t           pointer_mode,
				 uint8_t           keyboard_mode,
				 xcb_window_t      confine_to,
				 xcb_cursor_t      cursor,
				 xcb_timestamp_t   time)
{
	const auto &logger=real_pointer_grabObj::logger;

	returned_pointer<xcb_generic_error_t *> error;

	auto value=return_pointer
		(xcb_grab_pointer_reply
		 (c,
		  xcb_grab_pointer(c, owner_events, grab_window,
				   XCB_EVENT_MASK_BUTTON_PRESS |
				   XCB_EVENT_MASK_BUTTON_RELEASE |
				   XCB_EVENT_MASK_ENTER_WINDOW |
				   XCB_EVENT_MASK_LEAVE_WINDOW |
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

real_pointer_grabObj::real_pointer_grabObj(ONLY IN_THREAD,
				       const ref<generic_windowObj::handlerObj>
				       &grabbed_window)
	: grabbed_window(grabbed_window),
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

real_pointer_grabObj::~real_pointer_grabObj()
{
	if (result == XCB_GRAB_STATUS_SUCCESS)
		xcb_ungrab_pointer(conn()->conn, timestamp);
}

connection_info real_pointer_grabObj::conn() const
{
	return grabbed_window->screenref->get_connection()->impl->info;
}

bool real_pointer_grabObj::succeeded(ONLY IN_THREAD) const
{
	return result == XCB_GRAB_STATUS_SUCCESS;
}

void real_pointer_grabObj::allow_events(ONLY IN_THREAD)
{
	if (result == XCB_GRAB_STATUS_SUCCESS)
	{
		if (events_allowed)
			return;

		xcb_allow_events(conn()->conn, XCB_ALLOW_ASYNC_POINTER,
				 timestamp);
		events_allowed=true;
	}
}

element_implptr real_pointer_grabObj::get_grab_element(ONLY IN_THREAD)
{
	auto p=grabbing_element.getptr();

	if (!p)
		return element_implptr();

	return p->get_grab_element(IN_THREAD);
};

grabbed_pointerptr real_pointer_grabObj
::create_another_grab(ONLY IN_THREAD, const element_implptr &i)
{
	if (i)
	{
		auto grabbed=get_grab_element(IN_THREAD);

		if (grabbed)
			return grabbed_pointerptr();

		auto new_grab=
			ref<element_grabObj>::create(grabbed_pointer(this), i);

		grabbing_element=new_grab;

		return new_grab;
	}

	return grabbed_pointer(this);
}

///////////////////////////////////////////////////////////////////////////

grabbed_pointerptr elementObj::implObj::grab_pointer(ONLY IN_THREAD)
{
	if ( !data(IN_THREAD).logical_inherited_visibility ||
	     data(IN_THREAD).removed)
		return grabbed_pointerptr();

	return get_window_handler().grab_pointer(IN_THREAD, ref<implObj>(this));
}

grabbed_pointerptr generic_windowObj::handlerObj
::grab_pointer(ONLY IN_THREAD, const element_implptr &grabbing_element)
{
	auto &window_grab=current_pointer_grab(IN_THREAD);

	auto p=window_grab.getptr();

	if (p)
		return p->create_another_grab(IN_THREAD, grabbing_element);

	auto gp=ref<real_pointer_grabObj>::create(IN_THREAD,
						  ref<handlerObj>(this));

	if (!gp->succeeded(IN_THREAD))
		return grabbed_pointerptr();

	window_grab=gp;

	return gp->create_another_grab(IN_THREAD, grabbing_element);
}

bool generic_windowObj::handlerObj::is_pointer_actively_grabbed(ONLY IN_THREAD)
{
	return !!current_pointer_grab(IN_THREAD).getptr();
}

LIBCXXW_NAMESPACE_END
