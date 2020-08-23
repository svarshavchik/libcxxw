/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/background_color.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/text_hotspot.H"
#include "x/w/rgb_hash.H"

LIBCXXW_NAMESPACE_START

richtextmeta::richtextmeta(const background_color &textcolor,
			   const current_fontcollection &textfont)
	: textcolor(textcolor),
	  textfont(textfont)
{
}

richtextmeta::richtextmeta(const background_color &textcolor,
			   const current_fontcollection &textfont,
			   const background_colorptr &bg_color)
	: textcolor(textcolor),
	  textfont(textfont),
	  bg_color(bg_color)
{
}

richtextmeta::richtextmeta(const richtextmeta &)=default;

richtextmeta &richtextmeta::operator=(const richtextmeta &)=default;

richtextmeta::~richtextmeta()=default;

richtextmeta richtextmeta::replace_font(const current_fontcollection &font)
	const
{
	auto cpy=*this;

	cpy.textfont=font;

	return cpy;
}

std::strong_ordering richtextmeta::operator<=>(const richtextmeta &o) const
	noexcept
{
	std::strong_ordering ord=textcolor <=> o.textcolor;

	if (ord == std::strong_ordering::equal &&
	    (ord=bg_color <=> o.bg_color) == std::strong_ordering::equal &&
	    (ord=underline <=> o.underline) == std::strong_ordering::equal &&
	    (ord=rl <=> o.rl) == std::strong_ordering::equal &&
	    (ord=textfont <=> o.textfont) == std::strong_ordering::equal)
	{
		ord=link <=> o.link;
	}
	return ord;
}

LIBCXXW_NAMESPACE_END
