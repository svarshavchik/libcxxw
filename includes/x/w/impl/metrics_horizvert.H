/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_metrics_horizvert_H
#define x_w_impl_metrics_horizvert_H

#include <x/w/impl/metrics_horizvertfwd.H>
#include <x/w/impl/metrics_horizvertobj.H>
#include <x/w/alignment.H>
#include <x/w/rectangle.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

namespace metrics {
#if 0
}
#endif

//! Compute alignment

//! Used often in metrics-related context, so we'll throw this
//! declaration here.
//!
//! An widget whose halign and valign is as stated, and it's width
//! and height is as follows needs is to be positioned in an area
//! with the given outside width and height.
//!
//! Returns the adjusted rectangle for the element, suitably aligned
//! as follows:
//!
//! 1) If the corresponding alignment is a fill, and the widget size
//! is smaller, it is set to the outside size.
//!
//! 2) If the widget size is smaller and the corresponding alignment
//! is not a fill, the starting coordinate (x or y) is computed
//! accordingly (it is 0 by default).

rectangle align(dim_t outside_width,
		dim_t outside_height,
		dim_t element_width,
		dim_t element_height,
		halign h,
		valign v) LIBCXX_HIDDEN;

inline rectangle align(dim_t outside_width,
		       dim_t outside_height,
		       dim_t element_width,
		       dim_t element_height,
		       halign h,
		       valign v)
{
	rectangle r{0, 0, element_width, element_height};

	if (element_width < outside_width)
	{
		switch (h) {
		case halign::left:
			break;
		case halign::center:
			r.x=coord_t::truncate((outside_width-element_width)/2);
			break;
		case halign::right:
			r.x=coord_t::truncate(outside_width-element_width);
			break;
		case halign::fill:
			r.width=outside_width;
			break;
		}
	}

	if (element_height < outside_height)
	{
		switch (v) {
		case valign::top:
			break;
		case valign::middle:
			r.y=coord_t::truncate((outside_height - element_height)
					      / 2);
			break;
		case valign::bottom:
			r.y=coord_t::truncate(outside_height - element_height);
			break;
		case valign::fill:
			r.height=outside_height;
			break;
		}
	}

	return r;
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
