/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_grid_map_infofwd_h
#define x_w_grid_map_infofwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>

// #define LIBCXX_W_GRIDMAP_DEBUG

#ifdef LIBCXX_W_GRIDMAP_DEBUG
#include <x/stacktrace.H>
#include <x/ref.H>
#include <x/logger.H>
#include <string>
#include <list>
#endif

LIBCXXW_NAMESPACE_START

class grid_map_infoObj;

#ifdef LIBCXX_W_GRIDMAP_DEBUG

//! Mutex-protected internal grid data.

struct grid_map_t : public mpobj<ref<grid_map_infoObj>, std::recursive_mutex> {

	typedef mpobj<ref<grid_map_infoObj>, std::recursive_mutex> superclass_t;

	using superclass_t::superclass_t;

	std::list<std::string> locked_backtraces;

	struct lock : public superclass_t::lock {

		grid_map_t &locked;
		std::list<std::string>::iterator iter;

		lock(grid_map_t &gm) : superclass_t::lock{gm},
			locked{gm}
		{
			gm.locked_backtraces.push_back(stacktrace().backtrace);
			iter=--gm.locked_backtraces.end();
		}

		~lock() {
			locked.locked_backtraces.erase(iter);
		}
	};
};
#else

//! Internal structure used by the \ref gridlayoutmanager "grid layout manager".
typedef mpobj<ref<grid_map_infoObj>, std::recursive_mutex> grid_map_t;

#endif

LIBCXXW_NAMESPACE_END

#endif
