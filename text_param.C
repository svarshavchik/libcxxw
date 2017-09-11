/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "screen.H"
#include "messages.H"
#include "element.H"
#include "element_screen.H"
#include "background_color.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/picture.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtextstring.H"
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

text_decoration operator""_decoration(const char *s, size_t l)
{
	std::string_view n{s, l};

	if (n == "underline")
		return text_decoration::underline;

	return text_decoration::none;
}

text_param::text_param()=default;
text_param::~text_param()=default;

text_param::text_param(const text_param &)=default;
text_param &text_param::operator=(const text_param &)=default;

text_param &text_param::operator()(const std::string_view &s)
{
	std::u32string out_buf;

	unicode::iconvert::tou::convert(s.begin(), s.end(),
					unicode::utf_8, out_buf);

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

richtextstring elementObj::implObj::create_richtextstring(richtextmeta font,
							  const text_param &t,
							  bool allow_links)
{
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
	for (const auto &p:t.theme_colors)
		all_positions.insert(p.first);
	for (const auto &p:t.theme_background_colors)
		all_positions.insert(p.first);
	for (const auto &p:t.decorations)
		all_positions.insert(p.first);

	for (const auto &p:t.hotspots)
		all_positions.insert(p.first);

	if (!t.hotspots.empty())
		if (!allow_links)
			throw EXCEPTION(_("Links are not allowed in this text."));
	// Now iterate over them, in order, to create the unordered_map for
	// richtextstring's constructor.
	std::unordered_map<size_t, richtextmeta> m;

	bool inserted_default=false;

	auto screen_impl=get_screen()->impl;

	for (const auto &p:all_positions)
	{
		// If there is no initial font/color, insert one automatically.
		if (p > 0 && !inserted_default)
			m.insert({0, font});
		inserted_default=true;

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
				font=font.replace_font(create_font
						       (iter->second));
		}

		{
			auto iter=t.theme_fonts.find(p);

			if (iter != t.theme_fonts.end())
				font=font.replace_font(create_theme_font
						       (iter->second));
		}

		{
			auto iter=t.colors.find(p);

			if (iter != t.colors.end())
			{
				font.textcolor=screen_impl
					->create_background_color
					(screen_impl->create_solid_color_picture
					 (iter->second));
				font.bg_color=background_colorptr();
			}
		}

		{
			auto iter=t.theme_colors.find(p);

			if (iter != t.theme_colors.end())
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
					(screen_impl->create_solid_color_picture
					 (iter->second));
			}
		}

		{
			auto iter=t.theme_background_colors.find(p);

			if (iter != t.theme_background_colors.end())
				font.bg_color=screen_impl
					->create_background_color
					(iter->second);
		}

		{
			auto iter=t.hotspots.find(p);

			if (iter != t.hotspots.end())
				font.link=iter->second;
		}

		m.insert({p, font});
	}

	// If there were no font/color specifications, at all, and the string
	// is not empty, insert the default one.

	if (!inserted_default && !t.string.empty())
		m.insert({0, font});

	return {t.string, m};
}

LIBCXXW_NAMESPACE_END
