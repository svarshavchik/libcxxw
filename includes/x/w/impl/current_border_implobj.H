/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_current_border_implobj_h
#define x_w_impl_current_border_implobj_h

#include <x/w/impl/current_border_implfwd.H>
#include <x/w/impl/border_implfwd.H>
#include <x/w/impl/connection_threadfwd.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/border_arg.H>
#include <x/w/screenobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A current border.

//! \see current_border_impl

class current_border_implObj: virtual public obj {

public:
	//! My screen.
	const screen s;

private:
	//! The border argument specification.
	const border_arg arg;

	//! Current border.

	const_border_impl border_thread_only;

 public:

	THREAD_DATA_ONLY(border);

	//! Internal constructor
	current_border_implObj(const screen &s,
			       const border_arg &arg,
			       const current_theme_t::lock &lock)
		LIBCXX_HIDDEN;

 public:
	//! Destructor
	~current_border_implObj();

	//! This border's width or height is 0, or it has no colors
	bool no_border(ONLY IN_THREAD) const;

	//! The current theme has been updated.

	//! Check if the border has changed, and update it.
	//!
	//! Called from update_current_theme, while holding a lock.

	void current_theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
		LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
