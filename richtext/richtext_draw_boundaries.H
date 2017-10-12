/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution boundariesrmation.
*/

#ifndef richtext_draw_boundaries_H
#define richtext_draw_boundaries_H

#include "draw_infofwd.H"
#include "richtext/richtext_draw_boundariesfwd.H"
#include "x/w/rectangle.H"

LIBCXXW_NAMESPACE_START

//! Where richtextObj::draw() needs to draw its text.

//! Defines the boundaries for the redraw. The constructor takes the
//! text element's draw_info(), which specifies which portion of the text
//! element are visible where in the window, and a rectangular areas set,
//! what portions of the text element to redraw. The coordinates get
//! specified in absolute coordinates.
//!
//! The end result is draw_bounds, the bounding rectangle where the text
//! should be drawn, specified in coordinates relative to the text element's
//! (0, 0) coordinates.

struct LIBCXX_HIDDEN richtext_draw_boundaries {

	richtext_draw_boundaries(const draw_info &di,
				 const rectangle_set &areas);
	~richtext_draw_boundaries();

	// Viewable limits based on draw_info+rectangle_set
	rectangle limits;

	// Which portion of the text to draw.
	rectangle draw_bounds;

	inline bool nothing_to_draw() const
	{
		return draw_bounds.width == 0 || draw_bounds.height == 0;
	}
};

LIBCXXW_NAMESPACE_END;
#endif
