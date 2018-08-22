/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "ellipsiscache.H"
#include "richtext/richtext.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/element.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include <x/weakunordered_map.H>

LIBCXXW_NAMESPACE_START

size_t ellipsiscacheObj::ellipsis_cache_key_hash
::operator()(const ellipsis_cache_key_t &c) const
{
	return std::hash<current_fontcollection>::operator()(c.ellipsis_font)+
		std::hash<color_arg>::operator()(c.ellipsis_color);
}

bool ellipsiscacheObj::ellipsis_cache_key_t
::operator==(const ellipsis_cache_key_t &o) const
{
	return ellipsis_font == o.ellipsis_font &&
		ellipsis_color == o.ellipsis_color;
}

ellipsiscacheObj::ellipsiscacheObj()
	: cache{cache_t::create()}
{
}

ellipsiscacheObj::~ellipsiscacheObj()=default;

richtext ellipsiscacheObj::get(elementObj::implObj &parent_element)
{
	auto cfc=parent_element.create_current_fontcollection
		(theme_font{parent_element.label_theme_font()});
	color_arg color{parent_element.label_theme_color()};

	return cache->find_or_create
		({cfc, color},
		 [&]
		 {
			 return richtext::create
				 (parent_element.create_richtextstring
				  ({parent_element.create_background_color
				    (color), cfc},
					  U"\u2026"),
				  halign::left,
				  0);
		 });
}

LIBCXXW_NAMESPACE_END
