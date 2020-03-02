/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef cursor_pointer_cachefwd_H
#define cursor_pointer_cachefwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN cursor_pointer_cacheObj;

/*!

Cache of cursor pointers.

A per-screen cache of cursor_pointer objects.

*/

typedef ref<cursor_pointer_cacheObj> cursor_pointer_cache;

//! A nullable pointer reference to a \ref cursor_pointer_cache "cursor pointer cache".

typedef ptr<cursor_pointer_cacheObj> cursor_pointer_cacheptr;

//! A reference to a const \ref sxg "cursor pointer cache".

typedef const_ref<cursor_pointer_cacheObj> const_cursor_pointer_cache;

//! A nullable pointer reference to a const \ref sxg "cursor pointer cache".

typedef const_ptr<cursor_pointer_cacheObj> const_cursor_pointer_cacheptr;

LIBCXXW_NAMESPACE_END

#endif
