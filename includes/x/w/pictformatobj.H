/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_pictformatobj_H
#define x_w_pictformatobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/w/types.H>

#include <vector>

LIBCXXW_NAMESPACE_START

class drawableImplObj;

//! Rendering type

struct pictform_s {

	//! Indexed or direct mode
	bool indexed;
	//! Bitplane depth
	depth_t depth;

	//! Number of bits in the color component
	depth_t red_depth;
	//! Number of bits in the color component
	depth_t green_depth;
	//! Number of bits in the color component
	depth_t blue_depth;
	//! Number of bits in the color component
	depth_t alpha_depth;

	//! Color component bitmask position
	uint16_t red_shift;
	//! Color component bitmask
	uint16_t red_mask;
	//! Color component bitmask position
	uint16_t green_shift;
	//! Color component bitmask
	uint16_t green_mask;
	//! Color component bitmask position
	uint16_t blue_shift;
	//! Color component bitmask
	uint16_t blue_mask;
	//! Color component bitmask position
	uint16_t alpha_shift;
	//! Color component bitmask
	uint16_t alpha_mask;
};

//! Available picture formats

//! \internal

class pictformatObj : virtual public obj, public pictform_s {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.
	const ref<implObj> impl;

	class color_index;

	//! Color indexes, for indexed pictformats

	const std::vector<color_index> color_indexes;

	//! Constructor
	pictformatObj(const pictform_s &s, ref<implObj> &&implArg,
		      std::vector<color_index> &&color_indexesArg)
	LIBCXX_HIDDEN;

	//! Destructor
	~pictformatObj() LIBCXX_HIDDEN;

	//! Check if another pictformat has the same RGB components.

	bool rgb_compatible(const const_pictformat &format) const;

	// Search available_pictformats for all pictformats compatible
	// with the given one.
	//
	// This is typically used to select a pictformat for a top level
	// window. The screen's root window is an rgb window, and this is
	// going to find all pictformats that are rgb_compatible() with the
	// root window's

	std::list<const_pictformat> compatible_pictformats() const;

	//! Given a pictformat, return a compatible pictformat with an alpha channel.

	const_pictformat compatible_pictformat_with_alpha_channel() const;
};

#if 0
{
	{
#endif
	}
}

#endif
