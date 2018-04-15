/*
** Copyright 2017 Double Precision, Inc.
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

current_border_impl screenObj::implObj::get_cached_border(const border_arg &arg)
{
	return screen_border_cache->map->find_or_create
		(arg,
		 [&, this]
		 {
			 return ref<current_border_implObj>
				 ::create(ref<implObj>(this), arg);
		 });
}

LIBCXXW_NAMESPACE_END
