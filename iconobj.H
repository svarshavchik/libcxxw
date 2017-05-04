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

	//! The loaded image.

	const_icon_image image_thread_only;

public:

	THREAD_DATA_ONLY(image);

	//! Constructor

	iconObj(const const_icon_image &image);

	//! Destructor

	~iconObj();

	//! Finish icon's initialization.

	//! The default implementation calls theme_updated().

	virtual icon initialize(IN_THREAD_ONLY);

	//! Display theme was updated.

	//! The default implementation returns the same object.
	virtual icon theme_updated(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
