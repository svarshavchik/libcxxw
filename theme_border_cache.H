/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef theme_border_cache_h
#define theme_border_cache_h

#include "theme_border_cachefwd.H"
#include "current_border_implobj.H"
#include <x/weakunordered_multimap.H>
#include <x/ref.H>
#include <x/obj.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A cache of theme borders.

class LIBCXX_HIDDEN theme_border_cacheObj : virtual public obj {

public:
	typedef weakunordered_multimap<std::string,
				       current_border_implObj> map_t;

	map_t map;

	//! Constructor
	theme_border_cacheObj();

	//! Destructor
	~theme_border_cacheObj();
};

LIBCXXW_NAMESPACE_END

#endif
