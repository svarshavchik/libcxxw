/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drawable_H
#define drawable_H

#include "x/w/drawable.H"
#include "x/w/pictformatfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/screenfwd.H"
#include "x/w/dim_arg.H"
#include "iconfwd.H"
#include "xid_t.H"

#include <xcb/xproto.h>
#include <string>

LIBCXXW_NAMESPACE_START

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

	//! Create a new pixmap, using the same pictformat as this drawable.

	//! The new pixmap does not have any reference to this drawable.
	//! This drawable is used only to set the new pixmap's pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height);

	//! Create an alpha channel-only pixmap.

	//! Only depths of 1, 4, and 8 are supported.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     depth_t depth);

	//! Create a new pixmap, using a different pictformat.

	//! The new pixmap does not have any reference to this drawable.
	//! This drawable is used only to link the new pixmap to the same
	//! screen that this drawable is linked to.

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

	//! Create an icon, with dimensions specified in millimeters

	//! The created icon does not have any reference to this drawable.
	//! This drawable is used only to obtain the icon's pictformat, and
	//! create_pixmap().

	icon create_icon(const create_icon_args_t &);

 protected:

	//! Create an icon, with dimensions specified in millimeters

	//! Used by generic_windowObj::handlerObj before it is currently
	//! constructed, hence get_screen() is not yet overridden.

	icon create_icon(const create_icon_args_t &,
			 const screen &);
 public:
	//! Create an icon, with dimensions specified in pixels

	//! The created icon does not have any reference to this drawable.
	//! This drawable is used only to obtain the icon's pictformat, and
	//! create_pixmap().

	icon create_icon_pixels(const std::string &name,
				render_repeat icon_repeat,
				dim_t w, dim_t h, icon_scale scale);

	//! Create a vector of icons, with default dimensions.

	std::vector<icon>
		create_icon_vector(const std::vector<std::string> &images);
};

LIBCXXW_NAMESPACE_END

#endif
