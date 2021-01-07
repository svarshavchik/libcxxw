/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pixmap_loader_h
#define pixmap_loader_h

#include "pixmap.H"
#include "pictformat.H"

#include <xcb/xcb_image.h>

LIBCXXW_NAMESPACE_START

//! Load raw image data into a pixmap.

//! The pixmap gets constructed first. Then the pixmap_loader object
//! gets instantiated (in automated scope). put_rgb8() gets the RGBA values
//! of each pixel, and places it into an internally allocated buffer.
//!
//! flush() dumps the buffer into the pixmap.

class LIBCXX_HIDDEN pixmap_loader {

	//! Accumulated rgb lookup error.

	color_lookup_accumulated_error color_error;

public:
	//! Constructor.
	pixmap_loader(const pixmap &the_pixmap);

	//! Destructor.
	~pixmap_loader();

	//! Deleted copy constructor
	pixmap_loader(const pixmap_loader &)=delete;

	//! Deleted assignment operator

	pixmap_loader &operator=(const pixmap_loader &)=delete;

private:

	//! Pixmap being loaded.

	const pixmap the_pixmap;

	//! Pixmap width
	const dim_t width;

	//! Pixmap height
	const dim_t height;

	//! Pixmap's pictformat
	const const_pictformat pixmap_pictformat;

	//! Internal XCB library image data.
	xcb_image_t * const image;
public:

	//! Write an RGBA pixel value.

	//! We expect to write these in chronological order. If the pictformat
	//! is indexed, this is required for accumulated error correction to
	//! work correctly.

	inline void put_rgb8(dim_t x,
			     dim_t y,
			     unsigned char r,
			     unsigned char g,
			     unsigned char b,
			     unsigned char a)
	{
#define SCALE(n,mask,shift)                                             \
	(((((uint64_t)n)*(mask) + (255/2))/255)				\
	 << (shift))

		uint32_t v;

		if (pixmap_pictformat->indexed)
		{
			// Map to an indexed pixel value

			v=lookup_rgb(rgb(SCALE(r, rgb::maximum, 0),
					 SCALE(g, rgb::maximum, 0),
					 SCALE(b, rgb::maximum, 0),
					 SCALE(a, rgb::maximum, 0)),
				     pixmap_pictformat->color_indexes,
				     color_error);
		}
		else
		{
			v = SCALE(r, pixmap_pictformat->red_mask,
				  pixmap_pictformat->red_shift);
			v |= SCALE(g, pixmap_pictformat->green_mask,
				   pixmap_pictformat->green_shift);
			v |= SCALE(b, pixmap_pictformat->blue_mask,
				   pixmap_pictformat->blue_shift);
			v |= SCALE(a, pixmap_pictformat->alpha_mask,
				   pixmap_pictformat->alpha_shift);
		}
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			xcb_image_put_pixel(image,
					    (dim_t::value_type)x,
					    (dim_t::value_type)y,
					    v);
		}
	}

	//! Write the internal image buffer to the pixmap.
	void flush();
};

LIBCXXW_NAMESPACE_END

#endif
