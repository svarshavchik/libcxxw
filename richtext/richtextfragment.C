/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextfragment_render.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextcursorlocation.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtext_insert.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "richtext/richtext_linebreak_info.H"
#include "x/w/text_hotspot.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "x/w/impl/fonts/composite_text_stream.H"
#include "screen.H"
#include "x/w/impl/background_color.H"
#include "assert_or_throw.H"
#include "picture.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "x/w/impl/draw_info.H"
#include <x/sentry.H>
#include <cmath>
#include <courier-unicode.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::richtextfragmentObj);

LIBCXXW_NAMESPACE_START

#define USING_MY_PARAGRAPH() \
	assert_or_throw(my_paragraph && my_paragraph->my_richtext &&			\
			my_paragraph->fragments.size() > my_fragment_number,\
			"Internal error: paragraph or text linkage sanity check failed.")

#define RESOLVE_FONTS() \
	(string.resolve_fonts())

// Given a font height, compute how big the underline would be. A tiny one
// line for a huge font doesn't look good.

static dim_t underline_size(dim_squared_t font_height)
{
	font_height = (font_height + 10) / 20;

	if (font_height == 0)
		font_height=1;

	return dim_t::truncate(font_height);
}


// Even though this text range may not be underlined
// always take into account the underline's
// requirements. See underline().

dim_t adjust_descender_for_underline(dim_t ascender, dim_t descender)
{
	auto n=underline_size(ascender+descender);

	// Underline is positioned n rows
	// below the baseline, so the
	// total space required is n*2

	if (descender < n*2)
		descender=n*2;

	return descender;
}

richtextfragmentObj::richtextfragmentObj()
{
}

richtextfragmentObj
::richtextfragmentObj(const richtextstring &string,
		      size_t substr_pos,
		      size_t substr_len,
		      std::vector<unicode_lb>::const_iterator beg_breaks,
		      std::vector<unicode_lb>::const_iterator end_breaks)
	: string(string, substr_pos, substr_len),
	  breaks(beg_breaks, end_breaks)
{
}

void richtextfragmentObj::finish_setting()
{
	load_glyphs_widths_kernings(nullptr, nullptr);
}

richtextfragmentObj
::richtextfragmentObj(const richtextfragmentObj &current_fragment,
		      size_t substr_pos,
		      size_t substr_len)
	: string(current_fragment.string, substr_pos, substr_len),
	  breaks(current_fragment.breaks.begin()+substr_pos,
		 current_fragment.breaks.begin()+substr_pos+substr_len),
	  horiz_info(current_fragment.horiz_info, substr_pos, substr_len)
{
}



richtextfragmentObj::~richtextfragmentObj()
{
	for (const auto &location:locations)
		location->my_fragment=nullptr;
}


void richtextfragmentObj::get_fragment_out_of_bounds()
{
	throw EXCEPTION("Internal error: rich text fragment out of bounds.");
}

size_t richtextfragmentObj::y_position() const
{
	USING_MY_PARAGRAPH();

	return my_paragraph->first_fragment_y_position+y_pos;
}

std::pair<richtextfragmentObj *, bool>
richtextfragmentObj::find_y_position(size_t y_position_requested)
{
	std::pair<richtextfragmentObj *, bool> ret;
	auto y_position_value=y_position();
	auto my_fragment=this;

	ret.second=false;
	while (y_position_requested < y_position_value)
	{
		ret.second=true;
		auto p=my_fragment->prev_fragment();

		if (!p) break; // ???
		my_fragment=p;
		y_position_value -= dim_t::value_type(my_fragment->height());
	}

	if (ret.second)
	{
		ret.first=my_fragment;
		return ret;
	}

	auto height=my_fragment->height();

	while (y_position_value + dim_t::value_type(height)
	       <= y_position_requested)
	{
		ret.second=true;
		auto p=my_fragment->next_fragment();

		if (!p) break; // ...

		my_fragment=p;
		y_position_value += dim_t::value_type(height);
		height=my_fragment->height();
	}
	ret.first=my_fragment;
	return ret;
}

void richtextfragmentObj
::theme_updated_called_by_fragment_list(ONLY IN_THREAD,
					const const_defaulttheme &new_theme)
{
	string.theme_updated(IN_THREAD, new_theme);
	load_glyphs_widths_kernings();
	recalculate_size_called_by_fragment_list();
	redraw_needed=true;
}

void richtextfragmentObj::load_glyphs_widths_kernings()
{
	load_glyphs_widths_kernings(prev_fragment(), next_fragment());
}

void richtextfragmentObj
::load_glyphs_widths_kernings(richtextfragmentObj *previous_fragment,
			      richtextfragmentObj *next_fragment)
{
	USING_MY_PARAGRAPH();

	horiz_info.update
		([&, this]
		 (auto &widths,
		  auto &kernings)
		 {
			 string.compute_width((previous_fragment ?
					       &previous_fragment->string:NULL),
					      (next_fragment ?
					       &next_fragment->string:NULL),
					      my_paragraph->my_richtext
					      ->paragraph_embedding_level,
					      my_paragraph->my_richtext
					      ->unprintable_char,
					      widths,
					      kernings);
			  });
}

void richtextfragmentObj::update_glyphs_widths_kernings(size_t pos,
							size_t count)
{
	USING_MY_PARAGRAPH();

	auto previous_fragment=prev_fragment();
	auto next_fragment_=next_fragment();

	horiz_info.update
		([&, this]
		 (auto &widths,
		  auto &kernings)
		 {
			 string.compute_width((previous_fragment ?
					       &previous_fragment->string:NULL),
					      (next_fragment_ ?
					       &next_fragment_->string:NULL),
					      my_paragraph->my_richtext
					      ->paragraph_embedding_level,
					      my_paragraph->my_richtext
					      ->unprintable_char,
					      widths,
					      kernings,
					      pos, count);
			  });
}

richtextfragmentObj *richtextfragmentObj::prev_fragment() const
{
	USING_MY_PARAGRAPH();

	size_t n=my_fragment_number;

	if (n > 0)
		return &*my_paragraph->get_fragment(--n);

	auto paragraph_number=my_paragraph->my_paragraph_number;

	if (paragraph_number > 0)
	{
		auto &pr=**my_paragraph->my_richtext->paragraphs
			.get_paragraph(paragraph_number-1);

		assert_or_throw(!pr.fragments.empty(),
				"Internal error: paragraph with no fragments in prev_fragment()");

		auto last=pr.fragments.get_iter(pr.fragments.size()-1);

		return &**last;
	}

	return nullptr;
}

richtextfragmentObj *richtextfragmentObj::next_fragment() const
{
	USING_MY_PARAGRAPH();

	size_t n=my_fragment_number;

	if (++n < my_paragraph->fragments.size())
		return &*my_paragraph->get_fragment(n);

	auto paragraph_number=my_paragraph->my_paragraph_number;

	if (++paragraph_number < my_paragraph->my_richtext->paragraphs.size())
	{
		auto &pr=**my_paragraph->my_richtext->paragraphs
			.get_paragraph(paragraph_number);

		assert_or_throw(!pr.fragments.empty(),
				"Internal error: paragraph with no fragments in next_fragment()");

		auto first=pr.fragments.get_iter(0);

		return &**first;
	}

	return nullptr;
}

void richtextfragmentObj::recalculate_size_called_by_fragment_list()
{
	width=0;
	minimum_width=0;
	above_baseline=0;
	below_baseline=0;

	assert_or_throw(horiz_info.size() == breaks.size(),
			"Internal error: different width and breaks/kernings vectors");

	// Loop over the text string backwards, adding up the width and
	// kerning of each character (with the exceptions noted) to compute
	// the total width of the line. Also leverage this loop to compute
	// the minimum width of this line. This is used, to set the minimum
	// size of the rich text, which is the maximum minimum.

	dim_t width_so_far=0;

	initial_width=0;

	// If the fragment starts with rl text, find where the lr text starts.
	size_t lr_starts;

	{
		const auto &meta=string.get_meta();

		auto b=meta.begin(), e=meta.end();

		while (b != e && b->second.rl)
		{
			++b;
		}

		lr_starts= b == e ? string.size():b->first;
	}

	std::optional<dim_t> first_rl_width;

	for (size_t i=horiz_info.size(); i; )
	{
		--i;

		// Width of this character, together with the width of this
		// character plus its kerning from the previous character.

		auto c_width=horiz_info.width(i);
		auto c_total=c_width+horiz_info.kerning(i);

		// The kerning is not used for the first character.

		if (i == 0)
		{
			width = dim_t::truncate(width + c_width);
		}
		else
		{
			width = dim_t::truncate(width + c_total);
		}

		// We're seeing something in the line that can break here?

		if (i == 0 || breaks[i] != unicode_lb::none)
		{
			width_so_far =
				dim_t::truncate(width_so_far + c_width);

			if (width_so_far > minimum_width)
				minimum_width=width_so_far;

			// This loop iterates backwards. The last time we
			// get here we will compute the width of the
			// initial non-breakable sequence
			initial_width=width_so_far;

			// However if there's initial rl text, we want to
			// grab the first non-breakable sequence we see,
			// when we reach it (lr_starts is 0 if there are no
			// leading rl sections).
			if (i < lr_starts && !first_rl_width)
				first_rl_width=width_so_far;

			// Reset for the next one.
			width_so_far=0;
		}
		else
		{
			// Otherwize, the minimum width includes the kerning.
			width_so_far =
				dim_t::truncate(width_so_far + c_total);
		}
	}

	// Edge case: initial rl sequence.
	if (first_rl_width)
		initial_width=*first_rl_width;

	const auto &resolved_fonts=RESOLVE_FONTS();

	// Now go through the resolved fonts, determining the biggest ascender
	// and descender, to compute this fragment's ascender and descender.

	for (const auto &info:resolved_fonts)
	{
		const freetypefont &render_font=info.second;

		auto ascender=render_font->ascender;
		auto descender=adjust_descender_for_underline
			(render_font->ascender,
			 render_font->descender);

		if (above_baseline < ascender)
			above_baseline=ascender;
		if (below_baseline < descender)
			below_baseline=descender;
	}

	for (const auto &location:locations)
		location->horiz_pos_no_longer_valid();
}

size_t richtextfragmentObj::insert(ONLY IN_THREAD,
				   paragraph_list &my_paragraphs,
				   const richtext_insert_base &new_string)
{
	USING_MY_PARAGRAPH();

	auto pos=new_string.pos();
	const std::u32string &current_string=string.get_string();

	// Sanity checks
	assert_or_throw(pos <= current_string.size(),
			"Invalid pos parameter to insert()");

	auto n_size=new_string.size();

	if (n_size == 0) return 0; // Marginal

	// Make sure things will unwind properly, in the event of an unlikely
	// exception.

	auto string_sentry=
		make_sentry([&]
			    {
				    string.erase(pos, n_size);
			    });

	auto breaks_sentry=
		make_sentry([&]
			    {
				    breaks.erase(breaks.begin()+pos,
						 breaks.begin()+(pos+n_size));
			    });

	auto horiz_info_sentry=
		make_sentry([&]
			    {
				    horiz_info.erase(pos, pos+n_size);
			    });

	new_string(string);
	string_sentry.guard();

	// Make room for breaks, widths, and kernings.

	breaks.insert(breaks.begin()+pos, n_size, unicode_lb::none);
	breaks_sentry.guard();

	horiz_info.insert(pos, n_size);
	horiz_info_sentry.guard();
#if 0
	no_meta_for_trailing_space();
#endif

	update_glyphs_widths_kernings(pos, n_size);

	recalculate_linebreaks();

	fragment_list my_fragments{my_paragraphs, *my_paragraph};

	auto old_width=width;

	my_fragments.fragment_text_changed(my_fragment_number,
					   n_size);

	assert_or_throw(width >= old_width,
			"Internal error: inserting characters reduced text fragment width?");

	// Update cursor locations

	for (const auto &location:locations)
	{
		if (!location->do_not_adjust_in_insert)
		{
			location->inserted_at(IN_THREAD,
					      pos, n_size,
					      width-old_width);
		}
	}

	// At this point, it's new paragraphs, or bust!
	string_sentry.unguard();
	breaks_sentry.unguard();
	horiz_info_sentry.unguard();

	redraw_needed=true;

	size_t counter=1;
	for (size_t p=current_string.size(); p; )
	{
		if (breaks[--p] == unicode_lb::mandatory && p != 0)
		{
			split(my_fragments, p, split_lr);
			++counter;
		}
	}
	return counter;
}

// Recalculate line breaks for the previous fragment, this one,
// and the next fragment. We begin with the previous fragment,
// count the number of line breaks values to skip, how many line
// break values to count, then run all fragments through the
// linebreaking algorithm.

void richtextfragmentObj::recalculate_linebreaks()
{
	USING_MY_PARAGRAPH();

	const std::u32string &current_string=string.get_string();

	size_t skip=0;

	auto start_with=my_fragment_number;
	size_t n=1;

	if (start_with > 0)
	{
		--start_with;
		++n;
		skip += my_paragraph->get_fragment(start_with)->string.size();
	}

	auto end_with=my_fragment_number;

	if (++end_with < my_paragraph->fragments.size())
		++n;


	richtextstring *ptrs[n];

	for (size_t i=0; i<n; ++i)
	{
		ptrs[i]=&my_paragraph->get_fragment(start_with)->string;
		++start_with;
	}

	richtext_linebreak_info(skip, current_string.size(), &breaks[0],
				ptrs, n);

	start_with=my_fragment_number;
	if (start_with == 0)
	{
		// If this is the first fragment in the paragraph, and this
		// is not the first paragraph, the first character's line
		// breaking value must be unicode_lb::mandatory
		//
		// If this is the first fragment in paragraph 0, it's
		// uncode_lb::none (this logic is also present in do_set).

		breaks[0]=my_paragraph->my_paragraph_number > 0
			? unicode_lb::mandatory:unicode_lb::none;
	}
}

#if 0
richtextmetamap_t::iterator richtextfragmentObj::find_meta_for_pos(size_t pos)
{
	auto meta_iter=metadata.upper_bound(pos);

	assert_or_throw(meta_iter != metadata.begin(),
			"Internal error: upper_bound(pos) returned begin() in find_meta_for_pos().");

	return --meta_iter;
}

size_t richtextfragmentObj::meta_ending_pos(richtextmetamap_t::iterator iter)
{
	assert_or_throw(iter != metadata.end(),
			"Internal error: meta_ending_pos received ending iterator value");

	if (++iter == metadata.end())
		return string.size();

	return iter->first;
}

void richtextfragmentObj::no_meta_for_trailing_space()
{

	const std::u32string &current_string=string.get_string();

	if (next_fragment())
		return;

	auto last_location=current_string.size()-1;

	auto iter=find_meta_for_pos(last_location);

	if (iter->first == last_location)
		metadata.erase(iter);
}
#endif

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

coord_t richtextfragmentObj::first_xpos(ONLY IN_THREAD) const
{
	assert_or_throw(my_paragraph && my_paragraph->my_richtext,
			"Internal error: fragment not linked.");

	auto richtext=my_paragraph->my_richtext;
	auto text_width=richtext->width();

	if (richtext->text_width)
		text_width=richtext->text_width.value();

	if (width < text_width)
	{
		auto pad=dim_t::value_type(text_width-width);
		auto alignment=richtext->alignment;

		if (alignment == halign::center)
			return coord_t::truncate(pad/2);

		if (alignment == halign::right)
			return coord_t::truncate(pad);
	}
	else
	{
		auto extra=-coord_squared_t::truncate(width - text_width);
		auto alignment=richtext->alignment;

		if (alignment == halign::center)
			return coord_t::truncate(extra/2);

		if (alignment == halign::right)
			return coord_t::truncate(extra);
	}
	return 0;
}

dim_t richtextfragmentObj::x_width(ONLY IN_THREAD)
{
	assert_or_throw(my_paragraph && my_paragraph->my_richtext,
			"Internal error: fragment not linked.");

	return horiz_info.width();
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

////////////////////////////////////////////////////////////////////////////

void richtextfragmentObj::split(fragment_list &my_fragments, size_t pos,
				enum split_t type)
{
	USING_MY_PARAGRAPH();

	const std::u32string &current_string=string.get_string();

	assert_or_throw(pos > 0 && pos < current_string.size() &&
			breaks[pos] != unicode_lb::none,
			"Internal error: attempting to split a text fragment at a disallowed position");


	assert_or_throw(my_fragments.paragraph.my_paragraph_number ==
			my_paragraph->my_paragraph_number,
			"Internal error: wrong fragments parameter to split()");

	auto break_type=breaks[pos];

	// We can copy the relevant parts from myself, into a new fragment...
	auto new_fragment=richtextfragment::create(*this,
						   pos,
						   current_string.size()-pos);

	// ... then truncate myself accordingly.

	size_t n_erased=current_string.size()-pos;
	string.erase(pos, n_erased);
	breaks.erase(breaks.begin()+pos, breaks.end());
	horiz_info.erase(pos, n_erased);

	// We also must move any cursor locations to the new fragment.

	for (auto iter=locations.begin(); iter != locations.end(); )
	{
		auto p=iter;

		++iter;

		if ((*p)->get_offset() >= pos)
		{
			// This cursor location is now in the new fragment

			auto l= *p;

			new_fragment->locations.push_back(l);
			locations.erase(p);

			l->my_fragment=&*new_fragment;
			l->my_fragment_iter=--new_fragment->locations.end();
			l->split_from_fragment(pos);
		}
	}

	if (type == split_rl)
	{
		auto iter=my_paragraph->fragments.get_iter(my_fragment_number);

		my_fragments.insert_no_change_in_char_count(iter, new_fragment);
	}
	else if (break_type == unicode_lb::mandatory)
	{
		// Copy split content into a new paragraph.

		auto new_paragraph=my_fragments.my_paragraphs
			.insert_new_paragraph(my_paragraph->my_paragraph_number
					      +1);

		// This was split from here.
		fragment_list
			new_paragraph_fragments{my_fragments.my_paragraphs,
						*new_paragraph};

		// Move the remaining fragments to the new paragraph
		new_paragraph_fragments.split_from(new_fragment, this);
	}
	else
	{
		auto iter=my_paragraph->fragments.get_iter(my_fragment_number);

		++iter;

		my_fragments.insert_no_change_in_char_count(iter, new_fragment);
	}

	if (new_fragment->string.size())
		new_fragment->update_glyphs_widths_kernings(0, 1);

	my_fragments.fragment_text_changed(my_fragment_number, 0);

	redraw_needed=true;
}

void richtextfragmentObj::merge(fragment_list &my_fragments)
{
	USING_MY_PARAGRAPH();

	assert_or_throw(my_fragments.paragraph.my_paragraph_number ==
			my_paragraph->my_paragraph_number,
			"Internal error: wrong my_fragments parameter to split()");

	auto n=my_fragment_number;

	++n;

	if (n == my_paragraph->fragments.size())
		// Merge next paragraph's fragments?
		my_fragments.join_next();

	assert_or_throw(n != my_fragments.size(),
			"Internal error: merge() called on the last paragraph");

	// Look at the next fragment, and unceremoniously append it to this
	// fragment.

	auto other=my_paragraph->get_fragment(n);

	if (my_paragraph->my_richtext->paragraph_embedding_level ==
	    UNICODE_BIDI_LR)
	{
		if (string.get_dir() == richtext_dir::rl)
		{
			merge_lr_rl(my_fragments, other);
			return;
		}

		merge_lr_lr(my_fragments, other);
	}
	else
	{
		if (string.get_dir() == richtext_dir::lr)
		{
			merge_rl_lr(my_fragments, other);
			return;
		}

		other->merge_lr_lr(my_fragments, ref{this});
	}
}

void richtextfragmentObj::merge_rl_lr(fragment_list &my_fragments,
				      const richtextfragment &other)
{
	if (other->string.get_dir() == richtext_dir::lr)
	{
		merge_lr_lr(my_fragments, other);
		return;
	}

	auto &meta=other->string.get_meta();
	auto mb=meta.begin();
	auto me=meta.end();

	assert_or_throw(mb != me,
			"Internal error: empty meta in merge_rl_lr");

	auto p=me;

	while (p > mb && !p[-1].second.rl)
		--p;

	if (p == me)
	{
		// Next fragment ends with rl text, regular rl merge.

		other->merge_lr_lr(my_fragments, ref{this});
		return;
	}

	other->split(my_fragments, p->first, split_lr);

	other->merge_lr_lr(my_fragments, ref{this});

	auto next=other->next_fragment();

	assert_or_throw(next, "Internal error: did not find split fragment in"
			" merge_rl_lr");
	other->merge_lr_lr(my_fragments, ref{next});
}

void richtextfragmentObj::merge_lr_rl(fragment_list &my_fragments,
				      const richtextfragment &other)
{

	size_t next_left_to_right_start=
		other->string.left_to_right_start();

	if (next_left_to_right_start == 0)
	{
		merge_lr_lr(my_fragments, other);
		return;
	}

	bool merge_again=false;

	// If the following fragment has left-to-right text, split
	// it from it. From this point on, the following fragment has
	// only right to left text.

	if (other->string.get_string().size()
	    > next_left_to_right_start)
	{
		merge_again=true;
		other->split(my_fragments, next_left_to_right_start,
			     split_lr);
	}

	// Instead, merge this fragment at the end of the next one.
	other->merge_lr_lr(my_fragments, ref{this});

	if (merge_again)
	{
		// left-to-right text must follow, so this won't
		// recurse again.
		other->merge(my_fragments);
	}
}

void richtextfragmentObj::merge_lr_lr(fragment_list &my_fragments,
				      const richtextfragment &other)
{
	const std::u32string &current_string=string.get_string();

	auto orig_size=current_string.size();

	redraw_needed=true;

	if (!other->string.get_string().empty())
	{
		size_t orig_pos=current_string.size();

		breaks.reserve(breaks.size() + other->breaks.size());

		string.insert(orig_pos, other->string);

		breaks.insert(breaks.end(), other->breaks.begin(),
			      other->breaks.end());

		horiz_info.append(other->horiz_info);

		update_glyphs_widths_kernings(orig_pos, 1);
	}

	// Migrate the cursor locations too
	while (!other->locations.empty())
	{
		auto l=other->locations.front();

		locations.push_back(l);
		l->my_fragment=this;
		l->my_fragment_iter=--locations.end();
		l->merged_from_fragment(orig_size);
		other->locations.pop_front();
		l->split_from_fragment(0);
	}
	my_fragments.erase(my_paragraph->fragments
			   .get_iter(other->my_fragment_number));
	my_fragments.fragment_text_changed(my_fragment_number, 0);
}

void richtextfragmentObj::remove(size_t pos,
				 size_t nchars,
				 fragment_list &my_fragments)
{
	if (nchars == 0)
		return;

	USING_MY_PARAGRAPH();

	const std::u32string &current_string=string.get_string();

	assert_or_throw(pos < current_string.size(),
			"invalid starting position in remove()");

	assert_or_throw(current_string.size()-pos > nchars,
			"invalid character count in remove()");

	redraw_needed=true;

	// Remove all the bits and bytes

	string.erase(pos, nchars);

	breaks.erase(breaks.begin()+pos, breaks.begin()+pos+nchars);
	horiz_info.erase(pos, nchars);
	update_glyphs_widths_kernings(pos, 1);

	// Adjust all locations on or after the removal point.
	for (const auto &l:locations)
	{
		l->removed_from_fragment(pos, nchars);
	}

	recalculate_linebreaks();

	my_fragments.fragment_text_changed(my_fragment_number, -nchars);
}

void richtextfragmentObj
::move_locations_to_another_fragment(richtextfragmentObj *n)
{
	for (auto b=locations.begin(), e=locations.end(); b != e; )
	{
		auto p=b;

		++b;

		auto l=*p;

		l->my_fragment_iter=
			n->locations.insert(n->locations.end(), l);
		l->my_fragment=n;

		l->start_of_line();
		locations.erase(p);
	}
}

size_t richtextfragmentObj::index() const
{
	USING_MY_PARAGRAPH();

	size_t n=my_paragraph->first_fragment_n;

	return n + my_fragment_number;
}

///////////////////////////////////////////////////////////////////////////////

#if 0
void richtextfragmentObj::fragments_t
::append_rich_text(std::vector<unicode_char> &text,
		   richtextmetamap_t &meta)
{
	const std::u32string &current_string=string.get_string();

	for (const auto &fragment: static_cast<v_t &>(*this))
	{
		if (fragment->text.empty())
			continue; // Shouldn't happen

		auto p=current_string.size();

		text.insert(text.end(),
			    fragment->text.begin(),
			    fragment->text.end());

		if (meta.empty())
		{
			meta=fragment->metadata;
		}
		else
		{
			richtextmeta::insert(meta, p,
					     fragment->current_string.size(),
					     fragment->metadata);
		}
	}
}
#endif

// A thrown exception can end up nuking some fragments

void richtextfragmentObj::fragments_t::paragraph_destroyed()
{
	for (const auto &fragment: static_cast<v_t &>(*this))
		fragment->my_paragraph=nullptr;
}

richtextfragment richtextfragmentObj::fragments_t::find_fragment_for_pos(size_t &pos) const
{
	auto iter=std::lower_bound(begin(),
				   end(), pos,
				   []
				   (const richtextfragment &f, size_t pos)
				   {
					   return f->first_char_n <= pos;
				   });

	assert_or_throw(iter != begin(),
			"Internal error: empty list in find_fragment_for_pos");

	auto fragment=*--iter;

	pos -= fragment->first_char_n;

	size_t s=fragment->string.size();

	if (pos >= s)
	{
		assert_or_throw(s,
				"Internal error: empty fragment in find_fragment_for_pos()");
		pos=s-1;
	}

	return fragment;
}

LIBCXXW_NAMESPACE_END
