/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "background_color.H"
#include "fonts/current_fontcollection.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtextmetalink.H"
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

bool richtextmeta::operator==(const richtextmeta &o) const
{
	return textcolor==o.textcolor &&
		bg_color == o.bg_color &&
		underline==o.underline &&
		textfont == o.textfont &&
		link == o.link;

}

LIBCXXW_NAMESPACE_END