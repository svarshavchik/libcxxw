/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "x/w/impl/background_color.H"
#include "assert_or_throw.H"

#include <algorithm>

LIBCXXW_NAMESPACE_START

const richtextstring::resolved_fonts_t &richtextstring::resolve_fonts()
{
	if (!fonts_need_resolving)
		return resolved_fonts;

	coalesce();

	resolved_fonts.clear();

	auto last_font=resolved_fonts.end();

	// Find consecutive text segments that use the same font.

	auto textp=&*string.begin();
	for (auto b=meta.begin(), e=meta.end(); b != e; )
	{
		auto p=b;
		++b;

		size_t start_char=p->first;
		// From p->first, to end_char:
		size_t end_char= b == e ? string.size():b->first;

		assert_or_throw(end_char <= string.size(),
				"Internal error: text fragment metadata inconsistent");

		auto rb=textp+start_char;
		auto re=textp+end_char;

		// Now, look up the font for each character in the range
		// covered by this metadata entry.
		p->second.getfont()->fc_public.get()->lookup
			(rb, re,
			 [&, this]
			 (auto b,
			  auto e,
			  const freetypefont &font)
			 {
				 auto start_char=b-textp;

				 // If this is still the same font, just
				 // keep going.

				 if (last_font != resolved_fonts.end() &&
				     last_font->second == font)
					 return;

				 resolved_fonts.emplace_back(start_char, font);
				 last_font=--resolved_fonts.end();
			 }, ' ');
	}
	fonts_need_resolving=false;
	return resolved_fonts;
}

void richtextstring::compute_width(richtextstring *previous_string,
				   char32_t unprintable_char,
				   std::vector<dim_t> &widths,
				   std::vector<int16_t> &kernings)
{
	widths.resize(string.size());
	kernings.resize(string.size());

	compute_width(previous_string,
		      unprintable_char,
		      widths,
		      kernings,
		      0, string.size());
}

void richtextstring::compute_width(richtextstring *previous_string,
				   char32_t unprintable_char,
				   std::vector<dim_t> &widths,
				   std::vector<int16_t> &kernings,
				   size_t skip,
				   size_t count)
{
	assert_or_throw(skip <= string.size() &&
			count <= string.size()-skip,
			"Internal error: compute_width() parameters out of range");
	assert_or_throw(widths.size() == string.size() &&
			kernings.size() == string.size(),
			"Internal error: invalid buffer size in compute_width()");

	if (count == 0)
		return; // Optimization

	// Set end_skip to end of the range to compute.
	//
	// So our task at hand is to update widthd and kernings vectors
	// for [skip, end_skip).

	size_t end_skip=skip+count;

	// An insertion/removal of a character may affect the kerning of the
	// character after it, so do one more char, if possible.
	if (end_skip < string.size())
		++end_skip;

	const auto &fonts=resolve_fonts();
	char32_t previous_char=0;

	// For element #0, if the previous string ends in the same font,
	// set previous_char to the last character in the string, so that
	// we can set the kerning for element #0 accordingly.

	if (previous_string)
	{
		const auto &previous_fonts=
			previous_string->resolve_fonts();

		if (!previous_fonts.empty() && !fonts.empty() &&
		    (--previous_fonts.end())->second == fonts.begin()->second)
			previous_char=*previous_string->string.begin();
	}

	const char32_t *str=string.c_str();

	auto b=fonts.begin();

	if (skip > 0)
	{
		// If we're updating [skip, end_skip) and "skip" is not 0, we
		// can short-circuit and skip to the relevant entry in the
		// "fonts". array.
		//
		// We locate the font section that

		b=std::upper_bound(b, fonts.end(),
				   skip,
				   []
				   (size_t v, const auto &entry)
				   {
					   return v < entry.first;
				   });

		if (b != fonts.begin())
			--b;

		// If character #skip starts on a new font, we want to ignore
		// any previous_char we already set.

		if (b->first == skip)
			previous_char=0;
		else
			previous_char=string.at(skip-1);

	}

	for (auto p=b, e=fonts.end(); b != e; b=p)
	{
		++p;

		size_t start_char=b->first;
		size_t end_char=p == e ? string.size():p->first;

		assert_or_throw(end_char <= string.size() &&
				start_char <= end_char,
				"Internal error: invalid character range in compute_width()");
		if (start_char >= end_skip)
			break; // Optimization, we're done.

		if (skip > start_char)
		{
			assert_or_throw(skip < end_char,
					"Internal error in compute_width");

			// We start the calculation with the previous character,
			// so that when we get to character #skip we'll use the
			// kerning from the previous character. Below we'll
			// ignore the update of character #skip-1

			start_char=skip;
		}

		if (end_char > end_skip)
			end_char=end_skip;

		auto sb=str+start_char;
		auto se=str+end_char;

		b->second->load_glyphs(sb, se, unprintable_char);

		b->second->glyphs_size_and_kernings
			(sb, se,
			 [&]
			 (dim_t w,
			  dim_t h,
			  int16_t kerning_x,
			  int16_t kerning_y)
			 {
				 // Update the metrics ONLY for
				 // characters in the [skip, end_skip)
				 // range.
				 //
				 // We asserted that end_skip is
				 // less than the size of the vectors.

				 if (start_char >= skip &&
				     start_char < end_skip)
				 {
					 widths[start_char]=w;
					 kernings[start_char]=kerning_x;
				 }
				 ++start_char;
				 return true;
			 }, previous_char, unprintable_char);
		previous_char=0;
	}
}

void richtextstring::theme_updated(ONLY IN_THREAD,
				   const defaulttheme &new_theme)
{
	for (const auto &m:meta)
	{
		m.second.textcolor->theme_updated(IN_THREAD, new_theme);
		if (!m.second.bg_color.null())
			m.second.bg_color->theme_updated(IN_THREAD, new_theme);
	}

	// This is mostly to clear the cached resolved fonts:
	modified();
}

LIBCXXW_NAMESPACE_END
