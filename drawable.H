/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drawable_H
#define drawable_H

#include "x/w/drawable.H"
#include "x/w/pictformatfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/screenfwd.H"
#include "xid_t.H"

#include <xcb/xproto.h>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

//! Drawable implementation details.

class LIBCXX_HIDDEN drawableObj::implObj : virtual public obj {

 public:

	const connection_thread thread_;

	//! My xcb_drawable_t

	const xcb_drawable_t drawable_id;

	//! My pictformat

	const const_pictformat drawable_pictformat;

	//! Constructor

	implObj(//! My connection thread

		const connection_thread &thread_,

		//! My drawable
		const xcb_drawable_t drawable_id,

		//! Drawable's pixtformat.
		const const_pictformat &drawable_pictformat);

	~implObj();

	//! Create a new picture.

	picture create_picture();

	//! Create a new pixmap, using the same pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height);

	//! Create an alpha channel-only pixmap.

	//! Only depths of 1, 4, and 8 are supported.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     depth_t depth);

	//! Create a new pixmap, using a different pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     const const_pictformat &drawable_pictformat);

	//! Return my screen
	virtual screen get_screen()=0;

	//! Return my screen
	virtual const_screen get_screen() const=0;

	//! Return my width
	virtual dim_t get_width() const=0;

	//! Return my height
	virtual dim_t get_height() const=0;

	//! Return my depth
	depth_t get_depth() const;

	//! Compute depth of alpha channel for rendering fonts into this drawable.
	depth_t font_alpha_depth() const;
};

#if 0
{
	{
#endif
	}
}

#endif
