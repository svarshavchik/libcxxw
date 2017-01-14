/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowobj_h
#define x_w_generic_windowobj_h

#include <x/w/generic_windowfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/containerobj.H>
#include <x/w/drawableobj.H>
#include <x/obj.H>
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! Generic window facilities.

//! \see generic_window

class generic_windowObj : public containerObj,
			  public drawableObj {

public:
	LOG_CLASS_SCOPE;

	class implObj;
	class handlerObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	generic_windowObj(const ref<implObj> &impl,
			  const new_layoutmanager &layout_factory)
		LIBCXX_HIDDEN;

	//! Destructor
	~generic_windowObj() LIBCXX_HIDDEN;

	//! The window's screen.
	screen get_screen();

	//! The window's screen.
	const_screen get_screen() const;
};

LIBCXXW_NAMESPACE_END

#endif
