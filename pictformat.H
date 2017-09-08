/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pictformat_H
#define pictformat_H

#include "x/w/pictformat.H"
#include "connection_info.H"
#include <x/weaklist.H>
#include <xcb/xcb.h>
#include <xcb/render.h>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

typedef weaklist<pictformatObj> available_pictformats_t;

//! Pictformat implementation object.
class LIBCXX_HIDDEN pictformatObj::implObj : virtual public obj {

 public:
	//! This pictformat's connection

	const connection_info info;

	//! Identifier for this pictformat

	const xcb_render_pictformat_t id;

	//! Constructor
	implObj(const connection_info &info,
		const available_pictformats_t &available_pictformats,
		xcb_render_pictformat_t idArg);

	//! Destructor
	~implObj();

	//! Available pictformats
	const available_pictformats_t available_pictformats;
};

// Temporary structure used to distribute error values in rgb to indexed
// color mappings

struct LIBCXX_HIDDEN color_lookup_accumulated_error {

 public:
	int32_t err_r=0;
	int32_t err_g=0;
	int32_t err_b=0;
	int32_t err_a=0;

	color_lookup_accumulated_error() {}

	// Add accumulated error values to an rgb color, and keep it within
	// rgb boundaries.

	color_lookup_accumulated_error(const color_lookup_accumulated_error &e,
				       const rgb &color)
		: err_r(e.err_r+color.r),
		err_g(e.err_g+color.g),
		err_b(e.err_b+color.b),
		err_a(e.err_a+color.a)
		{
			if (err_r < 0) err_r=0;
			if (err_g < 0) err_g=0;
			if (err_b < 0) err_b=0;
			if (err_a < 0) err_a=0;

			if (err_r > rgb::maximum) err_r=rgb::maximum;
			if (err_g > rgb::maximum) err_g=rgb::maximum;
			if (err_b > rgb::maximum) err_b=rgb::maximum;
			if (err_a > rgb::maximum) err_a=rgb::maximum;
		}

	// Difference between accumulated color, and a given color.

	uint32_t error_value(const rgb &color)
	{
		int32_t diff_r=color.r-err_r;
		int32_t diff_g=color.g-err_g;
		int32_t diff_b=color.b-err_b;
		int32_t diff_a=color.a-err_a;

		if (diff_r < 0)
			diff_r= -diff_r;

		if (diff_g < 0)
			diff_g= -diff_g;

		if (diff_b < 0)
			diff_b= -diff_b;

		if (diff_a < 0)
			diff_a= -diff_a;

		return diff_r+diff_g+diff_b+diff_a;
	}
};

// Lookup the indexed pixel of a particular color amongst the list of
// colors in an indexed pictformat. This is done for an entire image at once,
// so we keep track of accumulated color correction error value.

inline uint32_t LIBCXX_HIDDEN
lookup_rgb(const rgb &color,
	   const std::vector<pictformatObj::color_index> &colors,
	   color_lookup_accumulated_error &error)
{
	color_lookup_accumulated_error lookup_color(error, color);

	if (colors.empty())
		return 0;

	uint32_t best_pixel=colors[0].index;
	rgb best_pixel_rgb=colors[0].color;

	uint32_t best_pixel_value=lookup_color.error_value(best_pixel_rgb);

	for (const auto &candidate:colors)
	{
		uint32_t value=lookup_color.error_value(candidate.color);

		if (value < best_pixel_value)
		{
			best_pixel=candidate.index;
			best_pixel_value=value;
			best_pixel_rgb=candidate.color;
		}
	}

	error.err_r -= best_pixel_rgb.r-color.r;
	error.err_g -= best_pixel_rgb.g-color.g;
	error.err_b -= best_pixel_rgb.r-color.b;
	error.err_a -= best_pixel_rgb.r-color.a;

	return best_pixel;
}

#if 0
{
	{
#endif
	}
}

#endif
