/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_attachedto_info.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

popup_attachedto_infoObj
::popup_attachedto_infoObj(const rectangle &attachedto_element_position,
			   const attached_to how)
	: attachedto_element_position_thread_only(attachedto_element_position),
	  how(how)
{
}

popup_attachedto_infoObj::~popup_attachedto_infoObj()=default;

dim_t popup_attachedto_infoObj
::max_peephole_width(IN_THREAD_ONLY,
		     const screen &screenref) const
{
	dim_t screen_width=screenref->impl->width_in_pixels();

	if (how == attached_to::submenu_next)
	{
		dim_t left_width=0;
		dim_t right_width=0;

		// Need to have meaningful response if the attached-to
		// element's position is weird.

		if (attachedto_element_position(IN_THREAD).x > 0 &&
		    dim_t::truncate(attachedto_element_position(IN_THREAD).x)
		    < screen_width)
			left_width=dim_t::truncate
				(attachedto_element_position(IN_THREAD).x);

		auto right_margin=attachedto_element_position(IN_THREAD).x
			+ attachedto_element_position(IN_THREAD).width;

		if (right_margin > 0)
		{
			dim_t x=dim_t::truncate(right_margin);

			if (x < screen_width)
				right_width=screen_width-x;
		}

		return left_width == 0 && right_width == 0 ? screen_width
			: left_width > right_width ? left_width:right_width;
	}
	return screen_width;
}

dim_t popup_attachedto_infoObj
::max_peephole_height(IN_THREAD_ONLY,
		      const screen &screenref) const
{
	auto screen_height=screenref->impl->height_in_pixels();

	if (how == attached_to::submenu_next)
		return screen_height;

	dim_t top_height=0;
	dim_t bottom_height=0;

	// Need to have meaningful response if the attached-to
	// element's position is weird.

	if (attachedto_element_position(IN_THREAD).y > 0 &&
	    dim_t::truncate(attachedto_element_position(IN_THREAD).y)
	    < screen_height)
		top_height=dim_t::truncate(attachedto_element_position(IN_THREAD).y);

	auto bottom_margin=attachedto_element_position(IN_THREAD).y
		+ attachedto_element_position(IN_THREAD).height;

	if (bottom_margin > 0)
	{
		dim_t y=dim_t::truncate(bottom_margin);

		if (y < screen_height)
			bottom_height=screen_height-y;
	}

	return top_height == 0 && bottom_height == 0 ? screen_height
		: top_height > bottom_height ? top_height:bottom_height;
}

LIBCXXW_NAMESPACE_END
