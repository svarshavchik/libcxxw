/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "assert_or_throw.H"

#include <algorithm>

LIBCXXW_NAMESPACE_START

const richtextstring::resolved_fonts_t &richtextstring::resolve_fonts() const
{
	if (!fonts_need_resolving)
		return resolved_fonts;

	coalesce();

	resolved_fonts.clear();

	auto last_font=resolved_fonts.end();

	// Find consecutive text segments that use the same font.

	auto textp=&*string.begin();

	auto p=meta.begin();
	for (auto b=meta.begin(), e=meta.end(); b != e; )
	{
		auto force_font_break=
			b->second.force_font_break(p->second);

		p=b;
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
				     last_font->second == font &&
				     !force_font_break)
					 return;

				 resolved_fonts.emplace_back(start_char, font);
				 last_font=--resolved_fonts.end();
			 }, ' ');
	}
	fonts_need_resolving=false;
	return resolved_fonts;
}

void richtextstring::theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
{
	// This is mostly to clear the cached resolved fonts:
	modified();
}

LIBCXXW_NAMESPACE_END
