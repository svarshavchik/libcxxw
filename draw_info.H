/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef draw_info_h
#define draw_info_h

#include "draw_infofwd.H"
#include "picture.H"
#include "x/w/rectangle.H"
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! Information that's needed to draw an element.

struct draw_info {
	//! The element is drawn in this window.
	picture_internal window_picture;

	//! The element is drawn in this boundary, inside window_picture
	rectangle viewport;

	//! The window's background color
	const_picture_internal window_background;

	//! Origin for the window_background.

	//! window_background's (0, 0) coordinates are located at
	//! at this position in viewport.
	coord_t background_x, background_y;
};

LIBCXXW_NAMESPACE_END

#endif
