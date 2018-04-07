/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/scratch_buffer_draw.H"
#include "x/w/impl/scratch_buffer.H"
#include "x/w/impl/child_element.H"
#include "generic_window_handler.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/gc.H"

LIBCXXW_NAMESPACE_START

template class scratch_buffer_draw<child_elementObj>;

scratch_buffer_draw_impl::scratch_buffer_draw_impl()=default;

scratch_buffer_draw_impl::~scratch_buffer_draw_impl()=default;

void scratch_buffer_draw_impl::get_scratch_buffer(ONLY IN_THREAD,
						  elementObj::implObj &element,
						  const draw_info &di)
{
	if (di.no_viewport())
		return;

	rectangle area_entire_rect{0, 0,
			di.absolute_location.width,
			di.absolute_location.height};

	clip_region_set clip{IN_THREAD, element.get_window_handler(), di};

	element.draw_using_scratch_buffer
		(IN_THREAD,
		 [&, this]
		 (const picture &area_picture,
		  const pixmap &area_pixmap,
		  const gc &area_gc)
		 {
			 do_draw(IN_THREAD, di,
				 area_picture, area_pixmap,
				 area_gc, clip, area_entire_rect);
		 },
		 area_entire_rect,
		 di,
		 di,
		 clip);
}

LIBCXXW_NAMESPACE_END
