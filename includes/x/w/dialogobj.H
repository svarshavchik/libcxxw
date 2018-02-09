/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dialogobj_h
#define x_w_dialogobj_h

#include <x/w/dialogfwd.H>
#include <x/w/main_windowfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see dialog

class dialogObj : virtual public obj {

public:
	class implObj;
	class handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! The dialog window

	const main_window dialog_window;

	//! Constructor
	dialogObj(const dialog_args &) LIBCXX_PUBLIC;

	//! Destructor
	~dialogObj() LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
