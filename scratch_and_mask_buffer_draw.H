/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_and_mask_buffer_draw_h
#define scratch_and_mask_buffer_draw_h

#include "scratch_buffer_draw.H"
#include "scratch_and_mask_buffer_drawfwd.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename child_type>
template<typename name1_type, typename ...Args>
scratch_and_mask_buffer_draw<child_type>
::scratch_and_mask_buffer_draw(name1_type &&mask_picture_name,
			       dim_t estimated_width,
			       dim_t estimated_height,
			       Args && ...args)
	: scratch_buffer_draw<child_type>
	(std::forward<Args>(args)...),
	scratch_and_mask_buffer_draw_impl
	(std::forward<name1_type>(mask_picture_name),
		 this->get_window_handler(),
		 estimated_width,
		 estimated_height)
{
}

//! Default destructor.

template<typename child_type>
scratch_and_mask_buffer_draw<child_type>::~scratch_and_mask_buffer_draw()
=default;

//! Reinvoke do_draw(), to draw into a scratch picture using a scratch mask.

template<typename child_type>
void scratch_and_mask_buffer_draw<child_type>
::do_draw(ONLY IN_THREAD,
	  const draw_info &di,
	  const picture &area_picture,
	  const pixmap &area_pixmap,
	  const gc &area_gc,
	  const rectangle &area_entire_rect)
{
	this->get_mask_scratch_buffer(IN_THREAD, di,
				      area_picture,
				      area_pixmap,
				      area_gc,
				      area_entire_rect);
}

LIBCXXW_NAMESPACE_END

#endif
