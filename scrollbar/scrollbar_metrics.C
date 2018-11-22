/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "scrollbar/scrollbar_metrics.H"
#include "x/w/rectangle.H"

LIBCXXW_NAMESPACE_START

scrollbar_metrics::scrollbar_metrics()=default;
scrollbar_metrics::~scrollbar_metrics()=default;
scrollbar_metrics::scrollbar_metrics(const scrollbar_metrics &)=default;
scrollbar_metrics &scrollbar_metrics::operator=(const scrollbar_metrics &)
=default;

void scrollbar_metrics::calculate(scroll_v_t scroll_low_size,
				  scroll_v_t scroll_high_size,
				  scroll_v_t virtual_size,
				  scroll_v_t page_size,
				  scroll_v_t pixel_size,
				  scroll_v_t minimum_handlebar_pixel_size)
{
	this->scroll_low_size=scroll_low_size;
	this->scroll_high_size=scroll_high_size;
	this->virtual_size=virtual_size;
	this->page_size=page_size;
	this->pixel_size=pixel_size;

	// assume the worst
	too_small=true;
	no_slider=true;
	handlebar_pixel_size=pixel_size;

	// careful with the overflow...
	if ( scroll_low_size >= pixel_size ||
	     pixel_size-scroll_low_size <= scroll_high_size) return;

	too_small=false;

	auto whatsleft=pixel_size-scroll_low_size-scroll_high_size;
	handlebar_pixel_size=whatsleft;

	if (page_size > virtual_size ||
	    whatsleft < minimum_handlebar_pixel_size)
		return;

	no_slider=false;

	//
	//      page_size         handlebar_pixel_size
	//    -------------   =  ----------------------
	//     virtual_size           whatsleft

	handlebar_pixel_size=
		scroll_v_t::truncate
		((scroll_squared_v_t::truncate(whatsleft) *
		  scroll_squared_v_t::truncate(page_size)) /
		 scroll_squared_v_t::truncate(virtual_size));

	if (handlebar_pixel_size < minimum_handlebar_pixel_size)
		handlebar_pixel_size=minimum_handlebar_pixel_size;
}

scroll_v_t scrollbar_metrics::value_to_pixel(scroll_v_t v) const
{
	if (no_slider)
		return 0;

	scroll_v_t v_range=virtual_size-page_size;
	scroll_v_t p_range=pixel_size-scroll_low_size-scroll_high_size
		-handlebar_pixel_size;

	if (v >= v_range)
		return p_range;

	//       v          p
	//    ------- =  -------
	//    v_range    p_range

	return scroll_v_t::truncate
		((scroll_squared_v_t::truncate(p_range) *
		  scroll_squared_v_t::truncate(v)) /
		 scroll_squared_v_t::truncate(v_range));
}

scrollbar_metrics::pixel_info scrollbar_metrics::pixel_to_value(scroll_v_t pixel)
	const
{
	pixel_info info;

	if (too_small)
		return info;

	if (pixel < scroll_low_size)
	{
		info.lo=true;
		return info;
	}

	if (pixel >= pixel_size - scroll_high_size)
	{
		info.hi=true;
		return info;
	}

	if (no_slider)
		return info;

	pixel -= scroll_low_size;

	// The pixel is within the slider area, but subtract half of the
	// handlebar_pixel_size of it, so that the original coordinate
	// represents the center of the handlebar.

	if (pixel < handlebar_pixel_size / 2)
		return info;
	pixel -= handlebar_pixel_size / 2;


	scroll_v_t p_range=pixel_size-scroll_low_size-scroll_high_size
		-handlebar_pixel_size;

	scroll_v_t v_range=virtual_size-page_size;

	if (pixel >= p_range)
	{
		info.value=v_range;
		return info;
	}

	//
	//        pixel         value
	//       -------   =  --------
	//       p_range       v_range

	info.value=scroll_v_t::truncate
		((scroll_squared_v_t::truncate(v_range) *
		  scroll_squared_v_t::truncate(pixel)) /
		 scroll_squared_v_t::truncate(p_range));
	return info;
}

void scrollbar_metrics::regions(rectangle &scroll_low,
				rectangle &slider,
				rectangle &scroll_high,
				coord_t rectangle::*coordinate,
				dim_t rectangle::*size) const
{
	if (too_small)
		return;

	scroll_low.*coordinate=0;
	scroll_low.*size=dim_t::truncate(scroll_low_size);

	scroll_high.*size=dim_t::truncate(scroll_high_size);
	scroll_high.*coordinate=coord_t::truncate(pixel_size-scroll_high_size);

	slider.*coordinate=coord_t::truncate(scroll_low_size);
	slider.*size=dim_t::truncate(pixel_size-scroll_low_size
				     -scroll_high_size);
}

LIBCXXW_NAMESPACE_END
