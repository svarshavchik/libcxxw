/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef icon_cachefwd_H
#define icon_cachefwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class icon_cacheObj;

/*!
Cache of icon-related objects.

A per-screen cache of icon objects.

*/

typedef ref<icon_cacheObj> icon_cache;

//! A nullable pointer reference to a \ref icon_cache "sxg cache".

typedef ptr<icon_cacheObj> icon_cacheptr;

//! A reference to a const \ref sxg "sxg cache".

typedef const_ref<icon_cacheObj> const_icon_cache;

//! A nullable pointer reference to a const \ref sxg "sxg cache".

typedef const_ptr<icon_cacheObj> const_icon_cacheptr;

LIBCXXW_NAMESPACE_END

#endif
