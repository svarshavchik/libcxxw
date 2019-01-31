/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_draw_info_h
#define x_w_impl_draw_info_h

#include <x/w/impl/draw_infofwd.H>
#include <x/w/picture.H>
#include <x/w/rectangle.H>

LIBCXXW_NAMESPACE_START

//! Information that's needed to draw an element.

//! This is based on: the element's position, its background color,
//! with the background color depending on its inherited_visibility
//! (which determines whether to use the element's defined backround color
//! or the parent element's background color).
//!
//! invalidate_cached_draw_info() gets invoked whenever any of those three
//! factors changes.

struct draw_info {

	//! Comparison operator

	inline bool operator==(const draw_info &o) const
	{
		return absolute_location == o.absolute_location &&
			element_viewport == o.element_viewport &&
			window_background_color == o.window_background_color &&
			background_x == o.background_x &&
			background_y == o.background_y;
	}

	//! Comparison operator
	inline bool operator!=(const draw_info &o) const
	{
		return !operator==(o);
	}

	//! The display element's position in the window

	//! This is, essentially, the sum of x/y coordinates of all elements
	//! starting with the top level window, whose x/y coordinates are
	//! logically (0,0), and ending with this display element.
	//!
	//! width and height of this is the element's current position's
	//! width and height.

	rectangle absolute_location;

	//! Return a rectarea containing just absolute_location, at (0, 0)

	//! This used to construct a rectarea to redraw this entire
	//! element.

	rectarea entire_area() const;

	//! The element is clipped to this viewport.

	//! It's possible that a display element is outside the visible
	//! region of its parent. The calculated visibly-clipped region is
	//! placed here. This is an empty set if this display element
	//! is completely invisible.
	//!
	//! This element's top/left coordinate in the top level window
	//! gets specified by absolute_location, and element_viewport
	//! is some subset (element_viewport's (0,0) is the top level window's
	//! top left corner).

	rectarea element_viewport;

	//! Everything is clipped.

	bool no_viewport() const
	{
		return element_viewport.empty();
	}

	//! The window's background color
	const_picture window_background_color;

	//! Origin for the window_background.

	//! window_background's (0, 0) coordinates are located at
	//! at this position in window_picture. If the background is this
	//! display element's, background_x and background_y are equal to
	//! absolute_location.[xy]

	coord_t background_x, background_y;

	//! Translate background coordinates relative to another set of coords.

	//! Essentially background_x and background_y from
	//! absolute_location.[xy]
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

	//! Translate background coordinates relative to another set of coords.

	//! \overload
	std::pair<coord_t, coord_t> background_xy_to(coord_t x,
						     coord_t y,
						     coord_t offset_x=0,
						     coord_t offset_y=0) const;
};

LIBCXXW_NAMESPACE_END

#endif
