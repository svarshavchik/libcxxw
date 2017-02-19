/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "scratch_and_mask_buffer_draw.H"
#include "scratch_buffer.H"
#include "child_element.H"
#include "generic_window_handler.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/pictformat.H"
#include "x/w/gc.H"

LIBCXXW_NAMESPACE_START

template class scratch_and_mask_buffer_draw<child_elementObj>;

scratch_and_mask_buffer_draw_impl
::scratch_and_mask_buffer_draw_impl(const std::string &label,
				    generic_windowObj::handlerObj &h,
				    dim_t estimated_width,
				    dim_t estimated_height)
	: mask_scratch(h.get_screen()
		       ->create_scratch_buffer
		       (label,
			h.get_screen()->find_alpha_pictformat_by_depth(1),
			estimated_width, estimated_height))
{
}

scratch_and_mask_buffer_draw_impl::~scratch_and_mask_buffer_draw_impl()=default;

void scratch_and_mask_buffer_draw_impl
::get_mask_scratch_buffer(IN_THREAD_ONLY,
			  const draw_info &di,
			  const picture &area_picture,
			  const pixmap &area_pixmap,
			  const gc &area_gc,
			  const rectangle &area_entire_rect)
{
	mask_scratch->get
		(di.absolute_location.width,
		 di.absolute_location.height,
		 [&, this]
		 (const picture &mask_picture,
		  const pixmap &mask_pixmap,
		  const gc &mask_gc)
		 {
			 do_draw(IN_THREAD,
				 di,
				 area_picture,
				 area_pixmap,
				 area_gc,
				 mask_picture,
				 mask_pixmap,
				 mask_gc,
				 area_entire_rect);
		 });
}



LIBCXXW_NAMESPACE_END
