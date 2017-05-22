/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef iconobj_H
#define iconobj_H

#include "iconfwd.H"
#include "icon_imagefwd.H"
#include "connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START;

//! An instantiated icon.

class LIBCXX_HIDDEN iconObj : virtual public obj {

public:

	//! The loaded image.

	const const_icon_image image;

	//! Constructor

	iconObj(const const_icon_image &image);

	//! Destructor

	~iconObj();

	//! Finish icon's initialization.

	//! The default implementation calls theme_updated().
	//!
	//! Because icon images are cached, a single icon's initialize()
	//! may be called multiple times, as each display element that
	//! receives its supposedly new icon takes care of its obligation
	//! to call initialize().

	virtual icon initialize(IN_THREAD_ONLY);

	//! Display theme was updated.

	//! The default implementation returns the same object.
	virtual icon theme_updated(IN_THREAD_ONLY);

	//! Resize this icon, if this is a resizable icon.
	virtual icon resizemm(IN_THREAD_ONLY, double widthmm, double heightmm);

	//! Resize this icon, if this is a resizable icon.
	virtual icon resize(IN_THREAD_ONLY, dim_t w, dim_t h);
};

LIBCXXW_NAMESPACE_END

#endif
