/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_iconobj_H
#define x_w_impl_iconobj_H

#include <x/w/defaultthemefwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/dim_arg.H>
#include <x/w/impl/iconfwd.H>
#include <x/w/impl/pixmap_with_picturefwd.H>
#include <x/w/impl/cursor_pointerfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START;

//! An instantiated icon.

class LIBCXX_HIDDEN iconObj : virtual public obj {

public:

	//! The loaded pixmap.

	const pixmap_with_picture image;

	//! Constructor

	iconObj(const pixmap_with_picture &image);

	//! Destructor

	~iconObj();

	//! Finish icon's initialization.

	//! The default implementation calls theme_updated().
	//!
	//! Because icon images are cached, a single icon's initialize()
	//! may be called multiple times, as each display element that
	//! receives its supposedly new icon takes care of its obligation
	//! to call initialize().

	virtual icon initialize(ONLY IN_THREAD)
		LIBCXX_PUBLIC;

	//! Display theme was updated.

	//! The default implementation returns the same object.
	virtual icon theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
		LIBCXX_PUBLIC;

	//! Resize this icon, if this is a resizable icon.
	virtual icon resize(ONLY IN_THREAD, dim_t w, dim_t h,
			    icon_scale scale) LIBCXX_PUBLIC;

	//! Create a cursor pointer from this icon.

	cursor_pointer create_cursor() LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
