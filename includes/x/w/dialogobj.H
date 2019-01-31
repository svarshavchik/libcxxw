/*
** Copyright 2017-2019 Double Precision, Inc.
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
	class LIBCXX_HIDDEN implObj;
	class LIBCXX_HIDDEN handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! The dialog window

	const main_window dialog_window;

	//! Constructor
	dialogObj(const dialog_args &) LIBCXX_PUBLIC;

	//! Destructor
	~dialogObj() LIBCXX_PUBLIC;

	//! Set dialog position with respect to its parent main window.

	void set_dialog_position(dialog_position);
};

LIBCXXW_NAMESPACE_END

#endif
