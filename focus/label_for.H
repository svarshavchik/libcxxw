/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_for_h
#define label_for_h

#include "x/w/impl/focus/label_forfwd.H"
#include "x/w/button_eventfwd.H"
#include "focus/label_forobj.H"

LIBCXXW_NAMESPACE_START

//! Keeps track of the button event processing.

struct button_event_redirect_info {

	//! A label_for widget has redirected this event.

	//! It won't be redirected again.
	bool event_redirected=false;

	//! The button event resulted in the focus being moved.

	//! It won't be moved again.
	bool focus_redirected=false;
};

LIBCXXW_NAMESPACE_END

#endif
