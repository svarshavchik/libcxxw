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

	//! Return a rectangle_set containing just absolute_location, at (0, 0)

	//! This used to construct a rectangle_set to redraw this entire
	//! element.

	rectangle_set entire_area() const;

	//! The element is clipped to this viewport.

	//! It's possible that a display element is outside the visible
	//! region of its parent. The calculated visibly-clipped region is
	//! placed here. This is an empty set if this display element
	//! is completely invisible.

	rectangle_set element_viewport;

	//! Everything is clipped.

	bool no_viewport() const
	{
		return element_viewport.empty();
	}

	//! The window's background color
	const_picture_internal window_background;

	//! Origin for the window_background.

	//! window_background's (0, 0) coordinates are located at
	//! at this position in window_picture. If the background is this
	//! display element's, background_x and background_y are equal to
	//! absolute_location.[xy]

	coord_t background_x, background_y;

	//! Translate background coordinates relative to another set of coords.

	//! Essentially subtract x/y from background_x and background_y.
	//!
	//! Optional offset is added to the resulting xy

	std::pair<coord_t, coord_t> background_xy_to(const draw_info &o,
						     coord_t offset_x=0,
						     coord_t offset_y=0) const
	{
		return background_xy_to(o.absolute_location.x,
					o.absolute_location.y,
					offset_x,
					offset_y);
	}

	std::pair<coord_t, coord_t> background_xy_to(coord_t x,
						     coord_t y,
						     coord_t offset_x=0,
						     coord_t offset_y=0) const;
};

LIBCXXW_NAMESPACE_END

#endif
