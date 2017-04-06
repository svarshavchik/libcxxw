/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextfragment_render_H
#define richtextfragment_render_H

#include "richtext/richtextfragment.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtextmetalinkfwd.H"
#include "x/w/picturefwd.H"
#include "draw_infofwd.H"

LIBCXXW_NAMESPACE_START

//! What's needed to render this fragment.

struct richtextfragmentObj::render_info {

	//! Scratch buffer for rendering the fragment.

	picture scratch_buffer;

	//! The draw_info for the display element being drawn.

	const draw_info &di;

	//! The draw_info for the background color's element

	//! scratch_buffer is already cleared to this background color

	const draw_info &background_color_di;

	//! The width of the scratch_buffer.

	//! The scratch buffer may be wider. This is what whoever ordered
	//! to renders ourselves tells us the actual width of the scratch
	//! buffer is.

	dim_t render_x_size;

	//! Added to the effective starting X coordinate.

	//! render() always renders text starting at column 0, in the
	//! scratch_buffer. But the starting logical X coordinate for
	//! rendering text is render_x_start. So, if render_x_start is
	//! -100, text starting at, and to the right of pixel #100 will
	//! be written (starting at X coordinate 0 in the scratch buffer).

	dim_t render_x_start=0;

	//! Added to the effective starting Y coordinate for the fragment.

	//! We always render into scratch_buffer at Y coordinate 0. This
	//! is used for background offset calculations.

	coord_t vert_scroll=0;

	//! Horizontal alignment
	halign alignment=halign::left;

	//! Used by richtexteditorwindow to highlight selected text
	size_t selection_start=0;

	//! Used by richtexteditorwindow to highlight selected text
	size_t selection_end=0;

	//! Link to highlight
	richtextmetalinkptr highlight_link;
};

//! Additional info for render_range
struct richtextfragmentObj::render_range_info {
	const struct render_info &info; //!< parameter to render()
	const char32_t *str; //!< Cached text.begin()
	coord_t &xpos; //!< Current X position, updated
	coord_t &ypos; //!< Current Y position, updated
	size_t start_char; //!< Starting char index in str to render
	size_t end_char; //!< Ending char index in str to render
	char32_t &prev_char; //!< Previous rendered char, updated
	const richtextmeta *font_info; //!< Metadata used for rendering
	const freetypefont *font; //!< The font used for rendering.
};

LIBCXXW_NAMESPACE_END

#endif
