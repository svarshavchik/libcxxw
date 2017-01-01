/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowobj_h
#define x_w_generic_windowobj_h

#include <x/w/generic_windowfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/elementobj.H>
#include <x/obj.H>
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! Generic window facilities.

//! \see generic_window

class generic_windowObj : public elementObj {

public:
	LOG_CLASS_SCOPE;

	class implObj;
	class handlerObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	generic_windowObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~generic_windowObj() noexcept LIBCXX_HIDDEN;

	//! The window's screen.
	screen get_screen();

	//! The window's screen.
	const_screen get_screen() const;
};

LIBCXXW_NAMESPACE_END

#endif
