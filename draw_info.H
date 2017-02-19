/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef draw_info_h
#define draw_info_h

#include "draw_infofwd.H"
#include "picture.H"
#include "x/w/rectangle.H"
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

//! Information that's needed to draw an element.

struct draw_info {
	//! The element is drawn in this window.
	picture_internal window_picture;

	//! The display element's position in the window

	//! This is, essentially, the sum of x/y coordinates of all elements
	//! starting with the top level window, whose x/y coordinates are
	//! logically (0,0), and ending with this display element.
	//!
	//! width and height of this is the element's current position's
	//! width and height.

	rectangle absolute_location;

	//! The element is clipped to this viewport.

	//! It's possible that a display element is outside the visible
	//! region of its parent. The calculated visibly-clipped region is
	//! placed here. This is an empty rectangle if this display element
	//! is completely invisible
	rectangle element_viewport;

	//! The window's background color
	const_picture_internal window_background;

	//! Origin for the window_background.

	//! window_background's (0, 0) coordinates are located at
	//! at this position in viewport.
	coord_t background_x, background_y;
};

LIBCXXW_NAMESPACE_END

#endif
