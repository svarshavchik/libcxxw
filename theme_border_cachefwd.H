/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef theme_border_cachefwd_h
#define theme_border_cachefwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class theme_border_cacheObj;

//! A cache for theme borders.

//! The purpose of the cache is mainly so that refs for the same border
//! will always be refs to the same object.

typedef ref<theme_border_cacheObj> theme_border_cache;

LIBCXXW_NAMESPACE_END

#endif
