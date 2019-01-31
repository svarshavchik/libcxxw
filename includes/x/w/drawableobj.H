/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_drawableobj_H
#define x_w_drawableobj_H

#include <x/obj.H>
#include <x/w/drawablefwd.H>
#include <x/w/picturefwd.H>
#include <x/w/pictformatfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/gcfwd.H>
#include <x/w/types.H>
#include <x/w/dim_arg.H>

LIBCXXW_NAMESPACE_START

//! A drawable object.

//! \internal

class drawableObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

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

	//! Create a new picture.

	//! load_pixmap() returns a const_drawable that overrides
	//! the const method to return the same picture object. This is an
	//! optimization.
	//!
	//! The default implementation creates a new picture, as usual.
	//! Because this one returns a const_picture, it cannot be used to
	//! modify the const pixmap.

	virtual const_picture create_picture() const;

	//! The drawable's screen.
	screen get_screen();

	//! The drawable's screen.
	const_screen get_screen() const;

	//! Create a new pixmap, using the same pictformat as this drawable.

	//! The new pixmap does not have any reference to this drawable.
	//! This drawable is used only to set the new pixmap's pictformat.

	pixmap create_pixmap(dim_t width,
			     dim_t height) const;

	//! Create a new pixmap, using a different pictformat.

	//! The new pixmap does not have any reference to this drawable.
	//! This drawable is used only to link the new pixmap to the same
	//! screen that this drawable is linked to.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     const const_pictformat &drawable_pictformat)
		const;

	//! Create an alpha channel-only pixmap.

	//! Only depths of 1, 4, and 8 are supported.

	pixmap create_pixmap(dim_t width,
			     dim_t height,
			     depth_t depth) const;

	//! Create a pixmap by loading an image file.

	//! Optionally specify the size of the loaded image, if the image
	//! uses the sxg format.
	//!
	//! Either the width or the height may be given as 0, which computes
	//! the size to preserve the image's aspect ratio.
	//!
	//! The size parameters get ignored for other
	//! image formats, and the image gets loaded using its native size.
	//!
	//! Returns a cached, const pixmap. The loaded image is not related
	//! to the drawable that created it.

	const_pixmap load_pixmap(const std::string &image,
				 const dim_arg &width=0,
				 const dim_arg &height=0) const;

	//! Create a pixmap by loading an image file.

	//! \overload
	//!
	//! Use a different pictformat to load the image.

	const_pixmap load_pixmap(const std::string &image,
				 const dim_arg &width,
				 const dim_arg &height,
				 const const_pictformat &drawable_pictformat)
		const;

	//! Create a pixmap by loading an image file.

	//! The size of the scaled loaded image gets specified in pixels.
	//!
	//! \see load_pixmap

	const_pixmap load_pixmap_pixels(const std::string &image,
					dim_t width,
					dim_t height) const;

	//! Create a pixmap by loading an image file.

	//! \overload
	//!
	//! Use a different pictformat to load the image.

	const_pixmap load_pixmap_pixels(const std::string &image,
					dim_t width,
					dim_t height,
					const const_pictformat
					&drawable_pictformat)
		const;



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
