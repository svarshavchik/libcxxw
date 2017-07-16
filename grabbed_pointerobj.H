/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grabbed_pointerobj_h
#define grabbed_pointerobj_h

#include "x/w/elementobj.H"
#include "elementfwd.H"
#include "x/w/generic_windowobj.H"
#include "connection_infofwd.H"
#include "connection_threadfwd.H"
#include <xcb/xproto.h>
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! A pointer grab attempt.

//! \see grabbed_pointer.

class LIBCXX_HIDDEN grabbed_pointerObj : virtual public obj {

	//! This is a popup pointer grab

	//! Which popup grabbed the pointer. Pointer events are redirected
	//! to this popup.
	weakptr<ptr<generic_windowObj::handlerObj>> grabbing_popup_thread_only;

 public:
	THREAD_DATA_ONLY(grabbing_popup);

	//! The logger
	LOG_CLASS_SCOPE;

	//! Which element grabbed me.

	weakptr<ptr<elementObj::implObj>> grabbing_element;

	//! Which top level window the grab is effective for.

	const ref<generic_windowObj::handlerObj> grabbed_window;
	//! Grabbed timestamp

	const xcb_timestamp_t timestamp;

	//! Result of the grab.
	const xcb_grab_status_t result;
	//! Constructor
	grabbed_pointerObj(IN_THREAD_ONLY,
			   const elementimplptr &grabbing_element,
			   const ref<generic_windowObj::handlerObj>
			   &grabbed_window);

private:

	connection_info conn() const;
 public:
	//! Destructor
	~grabbed_pointerObj();

	//! Whether the grab succeeded

	bool succeeded() const;
	//! Allow events

	void allow_events();
};

LIBCXXW_NAMESPACE_END

#endif
