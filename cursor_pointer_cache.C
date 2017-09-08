/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "cursor_pointer_cache.H"
#include "icon.H"
#include <x/refptr_hash.H>

LIBCXXW_NAMESPACE_START

cursor_pointer_cacheObj::cursor_pointer_cacheObj()
	: cache{cache_t::create()}
{
}

cursor_pointer_cacheObj::~cursor_pointer_cacheObj()=default;


cursor_pointer cursor_pointer_cacheObj
::create_cursor_pointer(const icon &i)
{
	return cache->find_or_create(i,
				     [&]
				     {
					     return cursor_pointer::create(i);
				     });
}

LIBCXXW_NAMESPACE_END
