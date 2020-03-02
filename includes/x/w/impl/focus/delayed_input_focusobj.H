/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_delayed_input_focusobj_h
#define x_w_delayed_input_focusobj_h

#include <x/w/impl/focus/delayed_input_focusfwd.H>
#include <x/w/impl/focus/focusable.H>
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see delayed_input_focus

class delayed_input_focusObj: virtual public obj {

	//! My focusable

	//! This object is owned by this focusable implementation object.
	weakptr<focusable_implptr> me_thread_only;

public:
	THREAD_DATA_ONLY(me);

	//! Constructor
	delayed_input_focusObj(const focusable_impl &me);

	//! Destructor
	~delayed_input_focusObj();
};

LIBCXXW_NAMESPACE_END

#endif
