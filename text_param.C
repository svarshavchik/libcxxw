/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "screen.H"
#include "messages.H"
#include "x/w/impl/element.H"
#include "x/w/impl/background_color.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/picture.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/text_hotspot.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

typedef int text_decoration_int_t;

theme_font operator"" _theme_font(const char *s, size_t l)
{
	return LIBCXXW_NAMESPACE::theme_font{{s, s+l}};
}

theme_color operator"" _color(const char *s, size_t l)
{
	return LIBCXXW_NAMESPACE::theme_color{{s, s+l}};
}

text_decoration operator"" _decoration(const char *s, size_t l)
{
	std::string_view n{s, l};

	if (n == "underline")
		return text_decoration::underline;

	return text_decoration::none;
}

text_param::text_param()=default;
text_param::~text_param()=default;

text_param::text_param(const text_param &)=default;
text_param::text_param(text_param &&)=default;

text_param &text_param::operator=(const text_param &)=default;
text_param &text_param::operator=(text_param &&)=default;

text_param &text_param::operator()(const std::string_view &s)
{
	std::u32string out_buf;

	unicode::iconvert::tou::convert(s.begin(), s.end(),
					unicode_locale_chset(), out_buf);

	string += out_buf;
	return *this;
}

text_param &text_param::operator()(const std::u32string_view &s)
{
	string.append(s.begin(), s.end());
	return *this;
}

text_param &text_param::operator()(const explicit_font &f)
{
	auto s=string.size();

	if (fonts.find(s) != fonts.end() ||
	    theme_fonts.find(s) != theme_fonts.end())
		throw EXCEPTION(gettextmsg("Duplicate font specification."));

	fonts.insert({s, f.f});
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

	if (colors.find(s) == colors.end())
		colors.insert({s, c});
	else
	{
		if (background_colors.find(s) != background_colors.end())
			throw EXCEPTION(gettextmsg("Duplicate color specification."));
		background_colors.insert({s, c});
	}
	return *this;
}

text_param &text_param::operator()(const theme_color &c)
{
	auto s=string.size();

	if (colors.find(s) == colors.end())
		colors.insert({s, c.name});
	else
	{
		if (background_colors.find(s) != background_colors.end())
			throw EXCEPTION(gettextmsg("Duplicate color specification."));
		background_colors.insert({s, c.name});
	}
	return *this;
}

text_param &text_param::operator()(const text_decoration d)
{
	auto s=string.size();

	auto iter=decorations.insert({s, text_decoration::none}).first;

	iter->second = (text_decoration)((text_decoration_int_t)iter->second
					 | (text_decoration_int_t)d);

	return *this;
}

text_param &text_param::operator()(const text_hotspot &h)
{
	if (!hotspots.insert({string.size(), h}).second)
		throw EXCEPTION(_("Duplicate text_hotspot specification"));

	return *this;
}

text_param &text_param::operator()(std::nullptr_t)
{
	if (hotspots.empty() || hotspots.find(string.size()) != hotspots.end()
	    || !hotspots.insert({string.size(), text_hotspotptr()}).second)
		throw EXCEPTION(_("Invalid text_hotspot specification"));

	return *this;
}

///////////////////////////////////////////////////////////////////////////
//
// Convert text_param into a richtextstring.

richtextstring elementObj::implObj
::create_richtextstring(const richtextmeta &default_font,
			const text_param &t,
			bool allow_links)
{
	richtextmeta font=default_font;

	// Compute all offsets into the richtextstring where fonts or colors
	// change.

	std::set<size_t> all_positions;

	for (const auto &p:t.fonts)
		all_positions.insert(p.first);
	for (const auto &p:t.theme_fonts)
		all_positions.insert(p.first);
	for (const auto &p:t.colors)
		all_positions.insert(p.first);
	for (const auto &p:t.background_colors)
		all_positions.insert(p.first);
	for (const auto &p:t.decorations)
		all_positions.insert(p.first);

	for (const auto &p:t.hotspots)
		all_positions.insert(p.first);

	if (!t.hotspots.empty())
	{
		if (!allow_links)
			throw EXCEPTION(_("Links are not allowed in this text."));
	}

	// We always have something at offset 0 if the string is not empty.

	if (t.string.size() > 0)
		all_positions.insert(0);

	if (allow_links)
		all_positions.insert(t.string.size());

	// Now iterate over them, in order, to create the unordered_map for
	// richtextstring's constructor.
	std::unordered_map<size_t, richtextmeta> m;

	auto screen_impl=get_screen()->impl;

	for (const auto &p:all_positions)
	{
		{
			auto iter=t.decorations.find(p);

			if (iter != t.decorations.end())
			{
				font.underline=false;

				if ( (text_decoration_int_t)iter->second &
				     (text_decoration_int_t)text_decoration
				     ::underline)
					font.underline=true;
			}
		}

		{
			auto iter=t.fonts.find(p);

			if (iter != t.fonts.end())
				font=font.replace_font
					(create_current_fontcollection
					 (iter->second));
		}

		{
			auto iter=t.theme_fonts.find(p);

			if (iter != t.theme_fonts.end())
				font=font.replace_font
					(create_current_fontcollection
					 (theme_font{iter->second}));
		}

		{
			auto iter=t.colors.find(p);

			if (iter != t.colors.end())
			{
				font.textcolor=screen_impl
					->create_background_color
					(iter->second);
				font.bg_color=background_colorptr();
			}
		}

		{
			auto iter=t.background_colors.find(p);

			if (iter != t.background_colors.end())
			{
				font.bg_color=screen_impl
					->create_background_color
					(iter->second);
			}
		}

		{
			auto iter=t.hotspots.find(p);

			if (iter != t.hotspots.end())
				font.link=iter->second;
		}

		m.insert({p, font});
	}

	if (allow_links)
		// richtextstring's constructor inserts a \0 here.
		// We made sure that all_positions includes t.string.size(),
		// above.
		m.find(t.string.size())->second=default_font;

	return {t.string, m, allow_links};
}

LIBCXXW_NAMESPACE_END
