/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "scratch_buffer_draw.H"
#include "scratch_buffer.H"
#include "child_element.H"
#include "generic_window_handler.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/gc.H"

LIBCXXW_NAMESPACE_START

template class scratch_buffer_draw<child_elementObj>;

scratch_buffer_draw_impl
::scratch_buffer_draw_impl(const std::string &label,
			   generic_windowObj::handlerObj &h,
			   dim_t estimated_width,
			   dim_t estimated_height)
	: area_scratch(h.get_screen()
		       ->create_scratch_buffer(label,
					       h.drawable_pictformat,
					       estimated_width,
					       estimated_height))
{
}

scratch_buffer_draw_impl::~scratch_buffer_draw_impl()=default;

void scratch_buffer_draw_impl::get_scratch_buffer(IN_THREAD_ONLY,
						  elementObj::implObj &element,
						  const draw_info &di,
						  const rectangle_set &areas)
{
	if (di.no_viewport())
		return;

	area_scratch->get
		(di.absolute_location.width,
		 di.absolute_location.height,
		 [&, this]
		 (const picture &area_picture,
		  const pixmap &area_pixmap,
		  const gc &area_gc)
		 {
			 rectangle area_entire_rect{0, 0,
					 di.absolute_location.width,
					 di.absolute_location.height};

			 // ok, di.absolute_location is our coordinate.

			 // di.background_[xy] is the background color's
			 // (0, 0). It follows that we copy
			 // absolute.[xy]-background_[xy].

			 area_picture->impl
				 ->composite(di.window_background,
					     coord_t::truncate(di.absolute_location.x-di.background_x),
					     coord_t::truncate(di.absolute_location.y-di.background_y),
					     area_entire_rect);

			 do_draw(IN_THREAD, di,
				 area_picture, area_pixmap,
				 area_gc, area_entire_rect);

			 elementObj::implObj::clip_region_set
				 clip{IN_THREAD, element, di};

			 di.window_picture->composite
				 (area_picture->impl,
				  0, 0,
				  di.absolute_location);
		 });
}

LIBCXXW_NAMESPACE_END
