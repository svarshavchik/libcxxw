/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "assert_or_throw.H"
#include <x/exception.H>

#include <algorithm>

LIBCXXW_NAMESPACE_START

void richtextstring::compute_width(const richtextstring *previous_string,
				   const richtextstring *next_string,
				   unicode_bidi_level_t embedding_level,
				   char32_t unprintable_char,
				   std::vector<dim_t> &widths,
				   std::vector<int16_t> &kernings)
{
	widths.resize(string.size());
	kernings.resize(string.size());

	compute_width(previous_string,
		      next_string,
		      embedding_level,
		      unprintable_char,
		      widths,
		      kernings,
		      0, string.size());
}

void richtextstring::compute_width(const richtextstring *previous_string,
				   const richtextstring *next_string,
				   unicode_bidi_level_t embedding_level,
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

	const auto &meta=get_meta();

	if (fonts.empty() || meta.empty())
		; // Edge case
	// Figure out what "previous_char" is, of character #0. For ordinary
	// left-to-right text, if the previous string ends in the same font,
	// set previous_char to the last character in the string, so that
	// we can set the kerning for element #0 accordingly.
	// This applies if this string beings with left-to-right text and
	// either:
	//
	// * paragraph direction is left-to-right, or
	//
	// * previous string is entirely left to right, and this one is too.
	//
	// Note that we check that this string begins with left-to-right
	// text, and that the previous string ends with left to right text.

	else if (previous_string &&
		 (embedding_level == UNICODE_BIDI_LR ||
		  (previous_string->get_dir() == richtext_dir::lr &&
		   get_dir() == richtext_dir::lr))
		 && !meta.begin()->second.rl)
	{
		const auto &previous_fonts=
			previous_string->resolve_fonts();
		const auto &previous_meta=
			previous_string->get_meta();
		if (!previous_fonts.empty() && !previous_meta.empty())
		{
			auto prev_last=--previous_meta.end();

			if (!prev_last->second.rl &&
			    (--previous_fonts.end())->second ==
			    fonts.begin()->second)
				previous_char=*--previous_string->string.end();
		}
	}

	// Otherwise, if the ENTIRE fragment is right-to-left, we look at the
	// next_string. If it starts with rl, we look at the last meta's

	else if (embedding_level == UNICODE_BIDI_LR &&
		 get_dir() == richtext_dir::rl && next_string)
	{
		const auto &next_fonts=next_string->resolve_fonts();
		resolved_fonts_t::const_iterator next_font_iter;

		auto index=next_string->left_to_right_start(&next_font_iter);

		if (next_font_iter != next_fonts.begin() &&
		    (--next_font_iter)->second == fonts.begin()->second)
		{
			previous_char=next_string->string.at(index-1);
		}
	}

	// Otherwise, if the paragraph embedding level is right-to-left
	// and this string starts with right-to-left text, we look at the
	// next_string, and what it ends with.
	else if (embedding_level != UNICODE_BIDI_LR && next_string)
	{
		auto b=get_meta().begin();

		auto &next_meta=next_string->get_meta();
		const auto &next_fonts=next_string->resolve_fonts();

		if (!next_meta.empty() && !next_fonts.empty())
			// Should always be the case.
		{
			auto e=--next_meta.end();

			if (b->second.rl && e->second.rl &&
			    fonts.begin()->second ==
			    (--next_fonts.end())->second)
			{
				previous_char=next_string->string.at
					(next_string->string.size()-1);
			}
		}
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

size_t richtextstring::left_to_right_start(resolved_fonts_t::const_iterator *p)
		const
{
	const auto &fonts=resolve_fonts();

	// Most common shortcuts:
	switch (get_dir()) {
	case richtext_dir::lr:
		if (p)
			*p=fonts.begin();
		return 0;
	case richtext_dir::rl:
		if (p)
			*p=fonts.end();
		return string.size();
	case richtext_dir::both:
		break;
	}

	const auto &meta=get_meta();

	auto b=meta.begin(), e=meta.end();

	auto lrp=std::find_if(b, e,
			      []
			      (auto &v)
			      {
				      return !v.second.rl;
			      });

	if (lrp == e)
		throw EXCEPTION("internal error, did not find left-to-right "
				"text");

	if (p)
	{
		// There should be a font break here, too.

		*p=std::find_if(fonts.begin(), fonts.end(),
				[&]
				(const auto &font_info)
				{
					return font_info.first == lrp->first;
				});
	}
	return lrp->first;
}

LIBCXXW_NAMESPACE_END
