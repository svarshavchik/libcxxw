/*
** Copyright 2017-2021 Double Precision, Inc.
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
#include <x/visitor.H>
#include <x/locale.H>

LIBCXXW_NAMESPACE_START

namespace {

	// Load the environment locale so that unicode_locale_chset()
	// uses the system character set.

	struct load_global_locale {

		load_global_locale()
		{
			locale::base::environment()->global();
		}
	};

	static load_global_locale do_load_global_locale;
}

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

bool text_param::undecorated() const
{
	return fonts.empty() && colors.empty() &&
		background_colors.empty() && decorations.empty() &&
		hotspots.empty();
}

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

text_param &text_param::operator()(explicit_font f)
{
	return operator()(font_arg{f.f});
}

text_param &text_param::operator()(const theme_font &f)
{
	return operator()(font_arg{f});
}

//! Use this font for all text going forward.
text_param &text_param::operator()(explicit_font_arg f)
{
	auto s=string.size();

	if (fonts.find(s) != fonts.end())
		throw EXCEPTION(gettextmsg("Duplicate font specification."));

	fonts.insert({s, f.f});
	return *this;
}

text_param &text_param::operator()(const text_color_arg &c)
{
	auto s=string.size();

	if (colors.find(s) == colors.end())
		colors.insert({s, c});
	else
	{
		if (background_colors.find(s) != background_colors.end())
			throw EXCEPTION(gettextmsg("Duplicate color specification."));
		std::visit(visitor{
				[&](const theme_color &c)
				{
					background_colors.insert({s, c.name});
				},
				[&](const rgb &c)
				{
					background_colors.insert({s, c});
				}}, c);
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
			hotspot_processing allow_links)
{
	richtextmeta font=default_font;

	// Compute all offsets into the richtextstring where fonts or colors
	// change.

	std::set<size_t> all_positions;

	for (const auto &p:t.fonts)
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
		if (allow_links != hotspot_processing::create)
			throw EXCEPTION(_("Links are not allowed in this text."));
	}

	// All text labels and input fields have a newline conveniently
	// appended to them. For input fields this gives a position for the
	// cursor after all entered text. There is no one-past-the-end
	// richtextiterator position.
	//
	// If this string is being created as updated hotspot contents:
	// we don't append an additional character here, the richtext will
	// take care of affixing separator markers to the hotspots. Instead
	// we just make sure that the replacement string is not empty.

	auto suffix=(allow_links == hotspot_processing::update
		     ? t.string.empty() ? U" ":U""
		     : U"\n");

	// As a consequence of this, we will always produce a non-empty
	// richtextstring here, and we always create metadata at position 0.

	all_positions.insert(0);

	// And we always create metadata at position string.size(), where
	// the trailing newline gets added. Except when we're creating
	// replacement hotspot text.
	if (allow_links != hotspot_processing::update)
		all_positions.insert(t.string.size());

	// Now iterate over them, in order, to create the unordered_map for
	// richtextstring's constructor.
	std::unordered_map<size_t, richtextmeta> m;

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
			auto iter=t.colors.find(p);

			if (iter != t.colors.end())
			{
				auto c=std::visit
					(visitor{
						[](const theme_color &c)
							->color_arg
						{
							return c.name;
						},
						[](const rgb &c)
							->color_arg
						{
							return c;
						}}, iter->second);

				font.textcolor=create_background_color(c);
				font.bg_color=background_colorptr();
			}
		}

		{
			auto iter=t.background_colors.find(p);

			if (iter != t.background_colors.end())
			{
				font.bg_color=create_background_color
					(iter->second);
			}
		}

		{
			auto iter=t.hotspots.find(p);

			if (iter != t.hotspots.end())
				font.link=iter->second;

			if (p == t.string.size())
			{
				// If the rich text string ends in a hotspot,
				// make sure that the trailing newline is NOT
				// a part of the hotspot!
				font.link=nullptr;
			}
		}

		m.insert({p, font});
	}

	// And the newline itself
	return {t.string + suffix, m};
}

LIBCXXW_NAMESPACE_END
