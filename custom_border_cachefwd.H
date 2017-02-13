/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_border_cachefwd_h
#define custom_border_cachefwd_h

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class custom_border_cacheObj;

//! A cache for custom borders.

//! The purpose of the cache is mainly so that refs to custom_border_impl
//! for the same border will always be refs to the same object.

typedef ref<custom_border_cacheObj> custom_border_cache;

LIBCXXW_NAMESPACE_END

#endif
