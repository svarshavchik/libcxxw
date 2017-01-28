/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_drawableobj_H
#define x_w_drawableobj_H

#include <x/obj.H>
#include <x/w/picturefwd.H>
#include <x/w/pictformatfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/types.H>

LIBCXXW_NAMESPACE_START

//! A drawable object.

//! \internal

class drawableObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details.
	const ref<implObj> impl;

	//! Constructor
	drawableObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~drawableObj() LIBCXX_HIDDEN;

	//! Return the drawable's pictformat.

	const_pictformat get_pictformat() const;

	//! Create a new picture.

	picture create_picture();

	//! The drawable's screen.
	screen get_screen();

	//! The drawable's screen.
	const_screen get_screen() const;

	//! Create a new pixmap, using the same pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height);

	//! Create a new pixmap, using a different pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     const const_pictformat &drawable_pictformat);

	//! Create an alpha channel-only pixmap.

	//! Only depths of 1, 4, and 8 are supported.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     depth_t depth);
};

#if 0
{
	{
#endif
	}
}

#endif
