/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef current_border_implobj_h
#define current_border_implobj_h

#include "current_border_implfwd.H"
#include "border_implfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/impl/defaultthemefwd.H"
#include "x/w/border_arg.H"
#include "x/w/screenobj.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A current border.

//! \see current_border_impl

class LIBCXX_HIDDEN current_border_implObj: virtual public obj {

	//! My screen.
	const ref<screenObj::implObj> screen;

	//! The border argument specification.
	const border_arg arg;

	//! Screen's current theme.

	defaulttheme current_theme_thread_only;

	//! Current border.

	const_border_impl border_thread_only;

	//! Whether it's been formally initialized

	bool initialized=false;
 public:

	THREAD_DATA_ONLY(current_theme);
	THREAD_DATA_ONLY(border);

	//! Constructor
	current_border_implObj(const ref<screenObj::implObj> &screen,
			       const border_arg &arg);

 private:

	//! Internal constructor
	current_border_implObj(const ref<screenObj::implObj> &screen,
			       const border_arg &arg,
			       const current_theme_t::lock &lock);

 public:
	//! Destructor
	~current_border_implObj();

	//! Initialize the border

	//! The element with the border is being initialized by the connection
	//! thread.

	void initialize(ONLY IN_THREAD);

	//! The current theme has been updated.

	//! Check if the border has changed, and update it.

	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme);
};

LIBCXXW_NAMESPACE_END

#endif
