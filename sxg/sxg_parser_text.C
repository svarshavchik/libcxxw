/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "sxg/sxg_parser.H"
#include "screen.H"
#include "screen_fontcaches.H"
#include "drawable.H"
#include "defaulttheme.H"
#include "x/w/impl/background_color.H"
#include "richtext/richtext.H"
#include "richtext/richtextmeta.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"

LIBCXXW_NAMESPACE_START

std::tuple<pixmap, picture>
sxg_parserObj::create_text_picture(const picture_info &info,
				   const drawable &main) const
{
	std::u32string s;
	std::unordered_map<size_t, richtextmeta> m;

	for (const auto &t:info.text_info)
	{
		if (t.text.empty())
			continue;

		font f;

		auto p=fonts.find(t.font);

		if (p != fonts.end())
			f=p->second;
		else
			f=theme->get_theme_font(t.font);

		if (f.scaled_size > 0)
		{
			f.pixel_size=dim_t::value_type(main->get_height())
				* f.scaled_size;
		}

		m.insert({s.size(),
					richtextmeta{
					screenref->impl
						->create_background_color
						(t.color.get_color
						 (main->get_width(),
						  main->get_height(),
						  main->impl->get_screen()
						  ->impl, theme)),
						screenref->impl->fontcaches
						->create_custom_font
						(screenref,
						 main->impl->font_alpha_depth(),
						 f)
						}});
		s += t.text;
	}

	if (m.empty())
		throw EXCEPTION("empty text picture string");

	// Use richtext to compute text dimensions, and render it.

	auto t=richtext::create(richtextstring{s, m}, info.align, 0);

	// Although we may not be in the connection thread, and probably are
	// not since, this richtext object is local only to this function
	// we can vouch to richtext that there won't be anyone else accessing
	// it.
	const auto &IN_THREAD=screenref->impl->thread;

	return t->create(IN_THREAD, main);
}

LIBCXXW_NAMESPACE_END
