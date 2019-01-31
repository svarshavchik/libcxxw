/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef scanline_H
#define scanline_H

#include "connection.H"

LIBCXXW_NAMESPACE_START

//! Calculate buffer used by scanline().

//! Before \ref glyphsetObj "glyphset code" calls scanline() to create the
//! bitmap of each one of the glyph's row, this calculates the size of the
//! buffer needed for each row, given the number
//! of pixels and number of bits per pixel.
//!
//! nbits is the number of bits per pixel: 1, 4, 8, 16, 24 or 32.
//!
//! 'nbits' must agree with setup.bitmap_format_scanline_unit. '16' is valid
//! only with a unit of 16 or 32 bits.
//! '24' and 32 is valid only with a unit of 32 bits.

template<typename npixels_t>
inline auto scanline_sizeof(npixels_t npixels, depth_t nbits,
			    const xcb_setup_t &setup)
{
	auto unit=setup.bitmap_format_scanline_unit;

	// How many pixels fit in each unit
	auto pixels_per_unit=unit/depth_t::value_type(nbits);

	// How many units are needed
	auto nunits=(npixels + (pixels_per_unit-1))/pixels_per_unit;

	// How many bytes are needed
	auto bytes=nunits * (unit/8);

	auto pad_bytes=setup.bitmap_format_scanline_pad/8;
	bytes += (pad_bytes-1);
	bytes -= (bytes % pad_bytes);
	return bytes;
}

//! Pack pixels into scanline order.

//! Calculate the size of output_buffer with scanline_sizeof()
//!
//! Send this function #npixels pixels, each pixel #nbits bits wide.
//!
//! The lambda takes a (typically size_t) parameter, and should return the
//! bits for pixel #i. Unused bits in the return value MUST be 0! So, if nbits
//! is 4, the return value must be 0 through 15.

template<typename get_lambda, typename npixels_t>
inline void scanline(uint8_t *output_buffer,
		     const npixels_t npixels,
		     depth_t nbits_t,
		     const xcb_setup_t &setup,
		     get_lambda &&get)
{
	const auto nbits=depth_t::value_type(nbits_t);
	uint32_t buffer=0;
	depth_t bits_done=0;

	const auto unit=setup.bitmap_format_scanline_unit;

	size_t j=0;

	for (npixels_t i=0;

	     // Iterate over all pixels, then keep going until we emit the
	     // last unit (and bits_done is 0).

	     i<npixels || bits_done != 0; ++i)
	{
		auto bits=i<npixels ? get(i):0;

		if (setup.bitmap_format_bit_order == XCB_IMAGE_ORDER_MSB_FIRST)
		{
			buffer=(buffer << nbits) | bits;
		}
		else
		{
			buffer=(buffer >> nbits) | (bits << (unit-nbits));
		}

		if ((bits_done += nbits) + nbits <= unit)
			continue;

		if (setup.image_byte_order==XCB_IMAGE_ORDER_MSB_FIRST)
		{
			switch (unit) {
			case 32:
				output_buffer[j++]=(buffer >> 24);
				// FALLTHRU
			case 24:
				output_buffer[j++]=(buffer >> 16);
				// FALLTHRU
			case 16:
				output_buffer[j++]=(buffer >> 8);
				// FALLTHRU
			case 8:
				output_buffer[j++]=(buffer);
			}
		}
		else
		{
			output_buffer[j++]=(buffer);
			if (unit > 8)
			{
				output_buffer[j++]=(buffer >> 8);
				if (unit > 16)
				{
					output_buffer[j++]=(buffer >> 16);
					if (unit > 24)
					{
						output_buffer[j++]=
							(buffer >> 24);
					}
				}
			}
		}
		buffer=0;
		bits_done=0;
	}

	auto pad_bytes=setup.bitmap_format_scanline_pad/8;

	size_t k=j+pad_bytes-1;

	k -= (k % pad_bytes);

	while (j<k)
		output_buffer[j++]=0;
}

LIBCXXW_NAMESPACE_END

#endif
