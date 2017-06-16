/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_grid_map_infofwd_h
#define x_w_grid_map_infofwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

class grid_map_infoObj;

//! Mutex-protected internal grid data.

typedef mpobj<ref<grid_map_infoObj>, std::recursive_mutex> grid_map_t;

LIBCXXW_NAMESPACE_END

#endif
