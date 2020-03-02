/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef border_cache_h
#define border_cache_h

#include "x/w/impl/current_border_implfwd.H"
#include "border_cachefwd.H"
#include "x/w/border_arg_hash.H"
#include <x/weakunordered_multimap.H>
#include <x/ref.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A cache of custom borders.

class border_cacheObj : virtual public obj {

public:
	typedef weakunordered_multimap<border_arg,
				       current_border_implObj> map_t;

	map_t map;

	//! Constructor
	border_cacheObj();

	//! Destructor
	~border_cacheObj();
};

LIBCXXW_NAMESPACE_END

#endif
