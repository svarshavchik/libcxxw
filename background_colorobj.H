/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_colorobj_H
#define background_colorobj_H

#include <x/obj.H>
#include "x/w/picturefwd.H"
#include "connection_threadfwd.H"
#include "defaultthemefwd.H"

LIBCXXW_NAMESPACE_START

//! An abstract handle for a "background colors".

//! A background color is just a picture. This abstracts away the picture
//! object, and implements initialize() and theme_updated().

class LIBCXX_HIDDEN background_colorObj : virtual public obj {

public:
	background_colorObj();
	~background_colorObj();

	virtual const_picture get_current_color(IN_THREAD_ONLY)=0;
	virtual void initialize(IN_THREAD_ONLY)=0;
	virtual void theme_updated(IN_THREAD_ONLY,
				   const defaulttheme &new_theme)=0;
};

LIBCXXW_NAMESPACE_END
#endif
