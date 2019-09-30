/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef cursor_pointer_cacheobj_H
#define cursor_pointer_cacheobj_H

#include "x/w/defaultthemefwd.H"
#include "cursor_pointer_cachefwd.H"
#include "cursor_pointerobj.H"
#include "x/w/impl/cursor_pointerfwd.H"
#include "x/w/impl/iconfwd.H"
#include <x/weakunordered_multimapfwd.H>

LIBCXXW_NAMESPACE_START;

//! A cache of cursor_pointers of various kinds.

//! This cache is attached to each screen object, and contains various cursor_pointer
//! image-related caches.

//! \see cursor_pointer_cache

class LIBCXX_HIDDEN cursor_pointer_cacheObj : virtual public obj {

 public:

	//! Constructor
	cursor_pointer_cacheObj();

	//! Destructor
	~cursor_pointer_cacheObj();

	//! Container for the cursor pointer cache.

	typedef weakunordered_multimap<icon, cursor_pointerObj> cache_t;

	//! The cursor pointer cache.

	const cache_t cache;

	//! Given an icon, create a cached cursor pointer for it.

	cursor_pointer create_cursor_pointer(const icon &);
};

LIBCXXW_NAMESPACE_END

#endif
