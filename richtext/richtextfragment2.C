/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextfragment_render.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextcursorlocation.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "x/w/impl/fonts/composite_text_stream.H"
#include "x/w/impl/background_color.H"
#include "picture.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "x/w/impl/draw_info.H"

LIBCXXW_NAMESPACE_START

// Draw background color before rendering text.

static inline void text_background(dim_t ascender_v,
				   dim_t descender_v,
				   const picture_internal &src,
				   coord_t background_x,
				   coord_t background_y,
				   const picture_internal &dst,
				   coord_t x1,
				   coord_t y1,
				   coord_t x2,
				   coord_t y2,
				   int64_t delta_x,
				   int64_t delta_y,
				   dim_t length_v)
{
	const dim_t::value_type
		ascender{ascender_v},
		descender{descender_v},
			length{length_v};

	auto below_direction_x=delta_y*descender/length;
	auto below_direction_y=delta_x*descender/length;

	coord_t x3=x1+below_direction_x;
	coord_t y3=y1+below_direction_y;
	coord_t x4=x2+below_direction_x;
	coord_t y4=y2+below_direction_y;

	auto above_direction_x=delta_y*ascender/length;
	auto above_direction_y=delta_x*ascender/length;

	x1 -= above_direction_x;
	y1 -= above_direction_y;

	x2 -= above_direction_x;
	y2 -= above_direction_y;

	picture::base::point points[]={
		{{x1}, {y1}},
		{{x2}, {y2}},
		{{x3}, {y3}},
		{{x4}, {y4}},
	};

	dst->fill_tri_strip(points, 4, src, render_pict_op::op_over,
			    coord_t::truncate(x1 + background_x),
			    coord_t::truncate(y1 + background_y));
}

// Underline text

static inline void underline(const picture_internal &src,
			     const picture_internal &dst,
			     coord_t x1,
			     coord_t y1,
			     coord_t x2,
			     coord_t y2,
			     int64_t delta_x,
			     int64_t delta_y,
			     dim_t length_v,
			     dim_t underline_thickness_v,
			     coord_t src_x,
			     coord_t src_y)
{
	const dim_t::value_type length{length_v},
		underline_thickness{underline_thickness_v};

	auto direction_x=delta_y*underline_thickness/length;
	auto direction_y=delta_x*underline_thickness/length;

	x1 += direction_x;
	y1 += direction_y;

	x2 += direction_x;
	y2 += direction_y;

	coord_t x3=x1+direction_x;
	coord_t y3=y1+direction_y;
	coord_t x4=x2+direction_x;
	coord_t y4=y2+direction_y;

	picture::base::point points[]={
		{{x1}, {y1}},
		{{x2}, {y2}},
		{{x3}, {y3}},
		{{x4}, {y4}},
	};

	dst->fill_tri_strip(points, 4, src, render_pict_op::op_over,
			    src_x, src_y);
}

// Draw custom text attributes

static inline void attributes(const richtextmeta &markup,
			      const freetypefont &font,
			      bool has_background_color,
			      const const_picture &foreground_color,
			      coord_t src_x,
			      coord_t src_y,
			      const const_picture &background_color,
			      coord_t background_x,
			      coord_t background_y,
			      const picture_internal &dst,
			      coord_t x1,
			      coord_t y1,
			      coord_t x2,
			      coord_t y2)
{
	if (!markup.underline && !has_background_color)
		return;

	// Most of this is pointless, at this time.

	int64_t delta_x=dim_squared_t::value_type(x2-x1);
	int64_t delta_y=dim_squared_t::value_type(y2-y1);

	double ddelta_x=delta_x;
	double ddelta_y=delta_y;

	dim_t length=dim_t::truncate
		((uint64_t)
		 std::trunc(std::sqrt(ddelta_x*ddelta_x + ddelta_y*ddelta_y)));

	if (length == 0)
		return; // Shouldn't happen

	// Draw the background color first
	if (has_background_color)
	{
		text_background(font->ascender,
				adjust_descender_for_underline(font->ascender,
							       font->descender),
				background_color->impl,
				background_x, background_y,
				dst, x1, y1, x2, y2,
				delta_x, delta_y, length);
	}

	// Then the underline

	if (markup.underline)
	{
		// Compute underline's thickness

		dim_t thickness=underline_size(font->ascender+
					       font->descender);

		underline(foreground_color->impl, dst, x1, y1, x2, y2,
			  delta_x, delta_y, length, thickness,
			  src_x, src_y);
	}
}

// Render text from the same font, and with the same colors.

inline void richtextfragmentObj
::render_range(render_range_info &range_info,
	       const richtextmeta &markup,
	       bool has_background_color,
	       const const_picture &foreground_color,
	       coord_t color_x,
	       coord_t color_y,
	       const const_picture &background_color,
	       coord_t background_x,
	       coord_t background_y)
{
	// Save starting coordinates (they get updated by glyphs_to_stream()
	auto start_x=range_info.xpos;
	auto start_y=range_info.ypos;

	composite_text_stream s((*range_info.font),
				range_info.end_char-range_info.start_char, 0);

	(*range_info.font)
		->glyphs_to_stream(range_info.str+range_info.start_char,
				   range_info.str+range_info.end_char, s,
				   range_info.xpos, range_info.ypos,
				   range_info.prev_char,
				   my_paragraph->my_richtext->unprintable_char);

	attributes(markup,
		   *range_info.font,
		   has_background_color,
		   foreground_color,
		   color_x,
		   color_y,
		   background_color,
		   background_x, background_y,
		   range_info.info.scratch_buffer->impl,
		   start_x, start_y,
		   range_info.xpos, range_info.ypos);

	s.composite(range_info.info.scratch_buffer,
		    foreground_color,
		    color_x,
		    color_y);

	range_info.prev_char=range_info.str[range_info.end_char-1];
}

// Combine the metadata range with overlayed attributes.

inline void richtextfragmentObj
::render_range_with_overlay(ONLY IN_THREAD,
			    render_range_info &range_info,
			    const overlay_map_t &overlay)
{
	// Interject overlay into range_info.start/end_char ranges.

	auto orig_end_char=range_info.end_char;

	while (range_info.start_char < orig_end_char)
	{
		auto iter=overlay.upper_bound(range_info.start_char);

		assert_or_throw(iter != overlay.begin(),
				"upper_bound() returned begin() in "
				"render_range_with_overlay()");

		range_info.end_char=orig_end_char;

		if (iter != overlay.end() && iter->first < orig_end_char)
			range_info.end_char=iter->first;
		// New overlay range starts within the range that's getting
		// printed.

		--iter;

		richtextmeta markup=*range_info.font_info;

		auto foreground_color=markup.textcolor
			->get_current_color(IN_THREAD);
		coord_t color_x=range_info.xpos;
		coord_t color_y=range_info.ypos;

		// We expect to render this text starting with row
		// y_position(). As such,
		bool has_background_color=!markup.bg_color.null();


		auto background_x=range_info.info.background_x;
		auto background_y=range_info.info.background_y;

		auto window_background_color=
			range_info.info.window_background_color;

		if (has_background_color)
		{
			background_x=range_info.info.absolute_x;
			background_y=range_info.info.absolute_y;

			window_background_color=
				markup.bg_color->get_current_color(IN_THREAD);
		}

		background_x=coord_t::truncate(background_x-
					       coord_t::value_type(range_info.info.absolute_x)-
					       coord_t::value_type(range_info.xpos)
					       );
		background_y=coord_t::truncate(background_y-
					       coord_t::value_type(range_info.info.absolute_y)-
					       coord_t::value_type(range_info.ypos));

		switch (iter->second) {
		case meta_overlay::normal:
			break;
		case meta_overlay::inverse:
			has_background_color=true;
			std::swap(color_x, background_x);
			std::swap(color_y, background_y);
			std::swap(foreground_color,
				  window_background_color);
			break;
		}
		render_range(range_info, markup,
			     has_background_color, foreground_color,
			     color_x, color_y,
			     window_background_color,
			     background_x, background_y);
		range_info.start_char=range_info.end_char;
	}
}

void richtextfragmentObj::overlay_merge(overlay_map_t &overlay,
					size_t start,
					size_t end,
					meta_overlay what)
{
	auto iter=overlay.upper_bound(end);

	assert_or_throw(iter != overlay.begin(),
			"overlay.upper_bound() returned begin()"
			" in overlay_merge()");
	auto resume=(--iter)->second; // When the overlay ends, return to this

	iter=overlay.upper_bound(start);

	assert_or_throw(iter != overlay.begin(),
			"overlay.upper_bound() returned begin()"
			" in overlay_merge()");

	--iter;

	// Remove anything between start and end.

	while (iter != overlay.end() && iter->first >= start
	       && iter->first < end)
	{
		auto orig=iter;
		++iter;
		overlay.erase(orig);
	}
	overlay.insert(std::make_pair(start, what));
	overlay.insert(std::make_pair(end, resume));
}

void richtextfragmentObj::render(ONLY IN_THREAD,
				 const render_info &info)
{
	USING_MY_PARAGRAPH();

	const std::u32string &current_string=string.get_string();

	if (current_string.size() == 0)
		return;

	auto str=current_string.c_str();

	overlay_map_t overlay;

	overlay.insert(std::make_pair(0, meta_overlay::normal));

	if (info.selection_end > info.selection_start)
	{
		assert_or_throw(info.selection_end <= current_string.size(),
				"Internal error: selected text range out of range.");

		overlay_merge(overlay, info.selection_start, info.selection_end,
			      meta_overlay::inverse);
	}
	for (const auto &location:locations)
	{
		const auto &l=*location;

		if (l.cursor_on)
		{
			auto offset=l.get_offset();

			overlay_merge(overlay,
				      offset,
				      offset+1,
				      meta_overlay::inverse);
		}
	}

	// Compute the real starting X coordinate using the alignment
	// (first_xpos), and adjust it by the horiz_scroll value.

	coord_squared_t x=first_xpos(IN_THREAD)-info.render_x_start;

	const auto &resolved_fonts=RESOLVE_FONTS();

	// Avoid spinning wheels if x is off the left margin. Adjust
	// start_char until we get to a visible character.

	size_t start_char=0;

	while (start_char + 1 < current_string.size())
	{
		auto next_x=x + horiz_info.width(start_char)
			+ horiz_info.kerning(start_char+1);

		if (next_x > 0)
			break;

		x=next_x;
		++start_char;
	}

	// Find start_char's font.

	auto resolved_font_iter=
		std::upper_bound(resolved_fonts.begin(),
				 resolved_fonts.end(),
				 start_char,
				 []
				 (size_t p, const auto &entry)
				 {
					 return p < entry.first;
				 });

	if (resolved_font_iter == resolved_fonts.begin())
		throw EXCEPTION("Internal error in render(): empty resolved font list");
	--resolved_font_iter;

	// We still need to look at the string metadata, for colors and such.

	const auto &meta=string.get_meta();

	auto meta_iter=std::upper_bound(meta.begin(),
					meta.end(),
					start_char,
					[]
					(size_t p, const auto &entry)
					{
						return p < entry.first;
					});

	if (meta_iter == meta.begin())
		throw EXCEPTION("Internal error: no meta in render()");

	--meta_iter;

	// THe logical ending coordinate.

	coord_squared_t end_x=
		coord_squared_t::truncate(info.render_x_size);

	coord_t ypos=info.ypos;

	// Keep rendering until we reach the end of the string...
	while (start_char < current_string.size())
	{
		// ... or the right margin (could be due to the horiz_scroll
		// offset).
		if (x >= end_x)
			break;

		char32_t prev_char=0;

		// If we started due to horiz_scroll in the middle of an
		// existing resolved font range, we have a previous character
		// to look at.
		if (start_char > resolved_font_iter->first)
			prev_char=current_string.at(start_char-1);

		auto font=resolved_font_iter->second;

		// This has been resolved until this ending character...

		++resolved_font_iter;

		size_t end_char= resolved_font_iter == resolved_fonts.end()
			? current_string.size()
			: resolved_font_iter->first;

		assert_or_throw(end_char > start_char,
				"Internal error: end_char <= start_char in render()");

		// ... so keep plugging away until the ending character is
		// reached.
		while (start_char < end_char)
		{
			// Has the next metadata range ends prematurely, before
			// the end_char?

			auto next_iter=meta_iter;
			++next_iter;

			auto range_end_char=next_iter==meta.end()
				? end_char:next_iter->first;

			assert_or_throw(range_end_char > start_char &&
					range_end_char <= end_char,
					"Internal error: inconsistent metadata in render()");

			// We now have all info needed to prepare a range_info
			// and call render_range_with_overlay().

			// x is coord_squared_t, for overflow purposes, but
			// range_info needs a ref to a coord_t.
			coord_t x_coord=coord_t::truncate(x);


			struct render_range_info range_info={
				.info=info,
				.str=str,
				.xpos=x_coord,
				.ypos=ypos,
				.start_char=start_char,
				.end_char=range_end_char,
				.prev_char=prev_char,
				.font_info=&meta_iter->second,
				.font=&font};

			render_range_with_overlay(IN_THREAD,
						  range_info,
						  overlay);

			x=coord_squared_t::truncate(range_info.xpos);
			ypos=range_info.ypos;
			prev_char=str[range_end_char-1];

			start_char=range_end_char;
			if (meta_iter + 1 != meta.end() &&
			    meta_iter[1].first <= start_char)
				++meta_iter;
		}
	}

	redraw_needed=false;
}

LIBCXXW_NAMESPACE_END
