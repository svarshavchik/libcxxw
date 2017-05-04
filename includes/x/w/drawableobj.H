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
#include <x/w/gcfwd.H>
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

	//! Create a new pixmap, using the same pictformat as this drawable.

	//! The new pixmap does not have any reference to this drawable.
	//! This drawable is used only to set the new pixmap's pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height);

	//! Create a new pixmap, using a different pictformat.

	//! The new pixmap does not have any reference to this drawable.
	//! This drawable is used only to link the new pixmap to the same
	//! screen that this drawable is linked to.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     const const_pictformat &drawable_pictformat);

	//! Create an alpha channel-only pixmap.

	//! Only depths of 1, 4, and 8 are supported.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     depth_t depth);

	//! Return my width
	dim_t get_width() const;

	//! Return my height
	dim_t get_height() const;

	//! Return my depth
	depth_t get_depth() const;

	//! Create a graphic context for this drawable.
	gc create_gc();
};

#if 0
{
	{
#endif
	}
}

#endif
