/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "border_cache.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "x/w/border_arg_hash.H"
#include "connection.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

border_cacheObj::border_cacheObj()
	: map{map_t::create()}
{
}

border_cacheObj::~border_cacheObj()=default;

current_border_impl get_cached_border(const screen &s,
				      const const_pictformat &pf,
				      const border_arg &arg)
{
	current_theme_t::lock lock{s->impl->current_theme};

	return s->impl->screen_border_cache->map->find_or_create
		(arg,
		 [&]
		 {
			 return ref<current_border_implObj>::create(s, pf, arg,
								    lock);
		 });
}

LIBCXXW_NAMESPACE_END
