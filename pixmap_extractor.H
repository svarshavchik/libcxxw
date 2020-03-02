/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pixmap_loader_h
#define pixmap_loader_h

#include "pixmap.H"
#include "pictformat.H"
#include "x/w/rgb.H"
#include <unordered_map>
#include <xcb/xcb_image.h>

LIBCXXW_NAMESPACE_START

//! Extract raw RGB image data from a pixmap.

//! After constructing the pixmap_extractor, get_rgb() returns a pixel's
//! color.

class LIBCXX_HIDDEN pixmap_extractor {

public:
	//! Constructor.
	pixmap_extractor(const const_pixmap &the_pixmap);

	//! Destructor.
	~pixmap_extractor();

	//! Deleted copy constructor
	pixmap_extractor(const pixmap_extractor &)=delete;

	//! Deleted assignment operator
	pixmap_extractor &operator=(const pixmap_extractor &)=delete;

private:

	//! Pixmap being extracted.

	const const_pixmap the_pixmap;

 public:
	//! Pixmap width
	const dim_t width;

	//! Pixmap height
	const dim_t height;

 private:
	//! Pixmap's pictformat
	const const_pictformat pixmap_pictformat;

	//! Color lookup for indexed pictformats
	const std::unordered_map<uint32_t, rgb> indexed_lookup;

	//! Internal XCB library image data.
	xcb_image_t * const image;

 public:

	//! I just want one lousy pixel.

	inline uint32_t get_pixel(dim_t x, dim_t y) const
	{
		if ( x >= width || y >= height)
			return 0;
		return xcb_image_get_pixel(image,
					   (dim_t::value_type)x,
					   (dim_t::value_type)y);
	}
	//! Retrieve a pixel.

	inline rgb get_rgb(dim_t x, dim_t y) const
	{
		if ( x >= width || y >= height)
			return {0, 0, 0, 0};

		auto pixel=xcb_image_get_pixel(image, (dim_t::value_type)x,
					       (dim_t::value_type)y);

		if (pixmap_pictformat->indexed)
		{
			auto i=indexed_lookup.find(pixel);

			return i == indexed_lookup.end()
				? rgb{0, 0, 0, 0}:i->second;
		}

		// Scale each RGB component value to the range 0-rgb::maximum.
		//
		// {red,green,blue,alpha} mask effectively gives us the
		// scaling ratio, since "mask" is the maximum value of the
		// color bits.
		//
		//    value           rgb_value
		//   -------    =    --------------
		//     mask           rgb::maximum
		//
		// So: shift the pixel by red/green/blue/alpha_shift bits,
		// mask off the corresponding {channel}_mask bits. This is
		// the "value".
		//
		// Multiply "value" by rgb::maximum, and divide by mask.

#define SCALE(mask,shift,default_value)	   \
		((rgb_component_t) \
			((mask) ? ((((pixel >> shift) & mask) *		\
				    (rgb_component_squared_t)rgb::maximum \
				    + mask / 2) / mask) : default_value))

		return {SCALE(pixmap_pictformat->red_mask,
			      pixmap_pictformat->red_shift, 0),
			SCALE(pixmap_pictformat->green_mask,
			      pixmap_pictformat->green_shift, 0),
			SCALE(pixmap_pictformat->blue_mask,
			      pixmap_pictformat->blue_shift, 0),
			SCALE(pixmap_pictformat->alpha_mask,
			      pixmap_pictformat->alpha_shift, rgb::maximum)};
	}
};

LIBCXXW_NAMESPACE_END

#endif
