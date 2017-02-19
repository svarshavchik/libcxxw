/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_buffer_draw_h
#define scratch_buffer_draw_h

#include "scratch_buffer_drawfwd.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"
#include "x/w/gc.H"
#include "draw_info.H"
#include "element.H"
#include "scratch_buffer.H"

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename child_type>
template<typename name_type, typename ...Args>
scratch_buffer_draw<child_type>
::scratch_buffer_draw(name_type &&name,
		      dim_t estimated_width,
		      dim_t estimated_height, Args && ...args)
	: child_type(std::forward<Args>(args)...),
	  scratch_buffer_draw_impl(std::forward<name_type>(name),
				   this->get_window_handler(),
				   estimated_width,
				   estimated_height)
{
}

	//! Default destructor.

template<typename child_type>
scratch_buffer_draw<child_type>::~scratch_buffer_draw()=default;

//! Reinvoke do_draw(), to draw into a scratch picture.

//! Before reinvoking do_draw(), the scratch picture gets cleared to the
//! draw_info's background color. After the wrapped do_draw() returns,
//! the area picture is copied into the window_picture, with draw_info's
//! clip region set.

template<typename child_type>
void scratch_buffer_draw<child_type>::do_draw(IN_THREAD_ONLY,
					      const draw_info &di,
					      const rectangle_set &areas)
{
	this->get_scratch_buffer(IN_THREAD, *this, di, areas);
}

LIBCXXW_NAMESPACE_END

#endif
