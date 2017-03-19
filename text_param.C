/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "screen.H"
#include "messages.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "richtext/richtextmeta.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

theme_font operator"" _theme_font(const char *s, size_t l)
{
	return LIBCXXW_NAMESPACE::theme_font{{s, s+l}};
}

theme_color operator"" _color(const char *s, size_t l)
{
	return LIBCXXW_NAMESPACE::theme_color{{s, s+l}};
}

text_param::text_param()=default;
text_param::~text_param()=default;

text_param::text_param(const text_param &)=default;
text_param &text_param::operator=(const text_param &)=default;

text_param &text_param::operator()(const std::experimental::string_view &s)
{
	std::u32string out_buf;

	unicode::iconvert::tou::convert(s.begin(), s.end(),
					unicode::utf_8, out_buf);

	string += out_buf;
	return *this;
}

text_param &text_param::operator()(const std::experimental::u32string_view &s)
{
	string.append(s.begin(), s.end());
	return *this;
}

text_param &text_param::operator()(const font &f)
{
	auto s=string.size();

	if (fonts.find(s) != fonts.end() ||
	    theme_fonts.find(s) != theme_fonts.end())
		throw EXCEPTION(gettextmsg("Duplicate font specification."));

	fonts.insert({s, f});
	return *this;
}

text_param &text_param::operator()(const theme_font &f)
{
	auto s=string.size();

	if (fonts.find(s) != fonts.end() ||
	    theme_fonts.find(s) != theme_fonts.end())
		throw EXCEPTION(gettextmsg("Duplicate font specification."));

	theme_fonts.insert({s, f.name});
	return *this;
}

text_param &text_param::operator()(const rgb &c)
{
	auto s=string.size();

	if (colors.find(s) == colors.end() &&
	    theme_colors.find(s) == theme_colors.end())
		colors.insert({s, c});
	else
	{
		if (background_colors.find(s) != background_colors.end() ||
		    theme_background_colors.find(s) !=
		    theme_background_colors.end())
			throw EXCEPTION(gettextmsg("Duplicate color specification."));
		background_colors.insert({s, c});
	}
	return *this;
}

text_param &text_param::operator()(const theme_color &c)
{
	auto s=string.size();

	if (colors.find(s) == colors.end() &&
	    theme_colors.find(s) == theme_colors.end())
		theme_colors.insert({s, c.name});
	else
	{
		if (background_colors.find(s) != background_colors.end() ||
		    theme_background_colors.find(s) !=
		    theme_background_colors.end())
			throw EXCEPTION(gettextmsg("Duplicate color specification."));
		theme_background_colors.insert({s, c.name});
	}
	return *this;
}

LIBCXXW_NAMESPACE_END
