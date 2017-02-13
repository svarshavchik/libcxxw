/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_border_cache_h
#define custom_border_cache_h

#include "current_border_implfwd.H"
#include "custom_border_cachefwd.H"
#include "x/w/border_infommfwd.H"
#include <x/weakunordered_multimap.H>
#include <x/ref.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A cache of custom borders.

class custom_border_cacheObj : virtual public obj {

public:
	typedef weakunordered_multimap<border_infomm,
				       current_border_implObj> map_t;

	map_t map;

	//! Constructor
	custom_border_cacheObj();

	//! Destructor
	~custom_border_cacheObj();
};

LIBCXXW_NAMESPACE_END

#endif
