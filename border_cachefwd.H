/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef border_cachefwd_h
#define border_cachefwd_h

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class border_cacheObj;

//! A cache for cborders.

//! The purpose of the cache is mainly so that refs to the same border_arg
//! will always be refs to the same object.

typedef ref<border_cacheObj> border_cache;

LIBCXXW_NAMESPACE_END

#endif
