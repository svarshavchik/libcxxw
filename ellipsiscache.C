/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "ellipsiscache.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/element.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/text_param.H"
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
	// Note that all of this is protected by current_theme_t::lock
	//
	// This is called from textlabelObj::implObj, with a current_theme_t
	// lock being for the duration of its construction. A lock is
	// also held by update_current_theme() while calling theme_updated()
	// on the cached ellipsis objects.

	auto cfc=parent_element.create_current_fontcollection
		(parent_element.label_theme_font());
	color_arg color{parent_element.label_theme_color()};

	return cache->find_or_create
		({cfc, color},
		 [&]
		 {
			 richtext_options options;

			 options.unprintable_char='.';

			 return richtext::create
				 (parent_element.create_richtextstring
				  ({parent_element.create_background_color
				    (color), cfc},
					  U"\u2026"),
				  options);
		 });
}

LIBCXXW_NAMESPACE_END
