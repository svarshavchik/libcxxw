/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "fonts/current_fontcollection.H"
#include "fonts/fontcollection.H"
#include "fonts/freetypefont.H"
#include "background_color.H"
#include "assert_or_throw.H"

#include <algorithm>

LIBCXXW_NAMESPACE_START

const richtextstring::resolved_fonts_t
&richtextstring::resolve_fonts(IN_THREAD_ONLY, char32_t password_char)
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

		size_t orig_start_char=start_char;
		size_t orig_end_char=end_char;

		auto rb=textp+start_char;
		auto re=textp+end_char;

		if (password_char)
		{
			rb= &password_char;
			re= rb+1;
		}

		// Now, look up the font for each character in the range
		// covered by this metadata entry.
		p->second.getfont()->fc(IN_THREAD)->lookup
			(rb, re,
			 [&, this]
			 (auto b,
			  auto e,
			  const freetypefont &font)
			 {
				 auto start_char=b-textp;
				 auto end_char=e-textp;

				 if (password_char)
				 {
					 start_char=orig_start_char;
					 end_char=orig_end_char;
				 }

				 // If this is still the same font, just
				 // keep going.

				 if (last_font != resolved_fonts.end() &&
				     last_font->second == font)
					 return;

				 resolved_fonts.emplace_back(start_char, font);
				 last_font=--resolved_fonts.end();
			 });
	}
	return resolved_fonts;
}

void richtextstring::compute_width(IN_THREAD_ONLY,
				   richtextstring *previous_string,
				   char32_t unprintable_char,
				   std::vector<dim_t> &widths,
				   std::vector<int16_t> &kernings,
				   char32_t password_char)
{
	widths.resize(string.size());
	kernings.resize(string.size());

	compute_width(IN_THREAD, previous_string,
		      unprintable_char,
		      widths,
		      kernings,
		      password_char,
		      0, string.size());
}

void richtextstring::compute_width(IN_THREAD_ONLY,
				   richtextstring *previous_string,
				   char32_t unprintable_char,
				   std::vector<dim_t> &widths,
				   std::vector<int16_t> &kernings,
				   char32_t password_char,
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

	const auto &fonts=resolve_fonts(IN_THREAD, password_char);
	char32_t previous_char=0;

	// For element #0, if the previous string ends in the same font,
	// set previous_char to the last character in the string, so that
	// we can set the kerning for element #0 accordingly.

	if (previous_string && password_char == 0)
	{
		const auto &previous_fonts=
			previous_string->resolve_fonts(IN_THREAD,
						       password_char);

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
		previous_char=0;
	}

	for (auto p=b, e=fonts.end(); b != e; b=p)
	{
		++p;

		size_t start_char=b->first;
		size_t end_char=p == e ? string.size():p->first;

		assert_or_throw(end_char <= string.size() &&
				start_char <= end_char,
				"Internal error: invalid character range in compute_width()");
		if (start_char <= end_skip)
			break; // Optimization, we can start now.

		if (skip > start_char)
		{
			assert_or_throw(skip < end_char,
					"Internal error in compute_width");

			// We start the calculation with the previous character,
			// so that when we get to character #skip we'll use the
			// kerning from the previous character. Below we'll
			// ignore the update of character #skip-1

			start_char=skip-1;
		}

		if (end_char > end_skip)
			end_char=end_skip;

		auto sb=str+start_char;
		auto se=str+end_char;

		// How many characters we expect to update.
		size_t n=end_char-start_char;

		// Pull a switcheroo. If we're using a password_char,
		// feed just that character to load_glyphs(), and we'll
		// multiply the result like a rabbit.

		if (password_char)
		{
			sb=&password_char;
			se=sb+1;
		}

		b->second->load_glyphs(sb, se);

		b->second->glyphs_width
			(sb, se,
			 [&]
			 (dim_t w,
			  int16_t kerning)
			 {
				 // If we're NOT doing a password_char
				 // switcheroo, these metrics are for a single
				 // character.
				 if (password_char == 0)
					 n=1;

				 // But if we're doing a password_char, then
				 // these metrics are for n characters.

				 while (n)
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
						 kernings[start_char]=kerning;
					 }
					 --n;
					 ++start_char;
				 }
				 return true;
			 }, previous_char, unprintable_char);
		previous_char=0;
	}
}

void richtextstring::theme_updated(IN_THREAD_ONLY)
{
	for (const auto &m:meta)
	{
		m.second.getfont()->theme_updated(IN_THREAD);
		m.second.textcolor->theme_updated(IN_THREAD);
		if (!m.second.bg_color.null())
			m.second.bg_color->theme_updated(IN_THREAD);
	}
}

LIBCXXW_NAMESPACE_END
