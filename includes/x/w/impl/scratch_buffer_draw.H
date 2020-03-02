/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scratch_buffer_draw_h
#define scratch_buffer_draw_h

#include <x/w/picture.H>
#include <x/w/pixmap.H>
#include <x/w/gc.H>
#include <x/w/impl/draw_info.H>
#include <x/w/impl/scratch_buffer_drawfwd.H>
#include <x/w/impl/scratch_buffer.H>

LIBCXXW_NAMESPACE_START

//! Default destructor.

template<typename child_type>
scratch_buffer_draw<child_type>::~scratch_buffer_draw()=default;

//! Reinvoke do_draw(), to draw into a scratch picture.

//! Before reinvoking do_draw(), the scratch picture gets cleared to the
//! draw_info's background color. After the wrapped do_draw() returns,
//! the area picture is copied into the window_picture, with draw_info's
//! clip region set.

template<typename child_type>
void scratch_buffer_draw<child_type>::do_draw(ONLY IN_THREAD,
					      const draw_info &di,
					      const rectarea &areas)
{
	this->get_scratch_buffer(IN_THREAD, *this, di);
}

LIBCXXW_NAMESPACE_END

#endif
