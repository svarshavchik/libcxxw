/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_grid_H
#define metrics_grid_grid_H

#include <map>
#include <list>
#include <string>
#include "metrics_grid_posfwd.H"

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! A container of elements arranged in a grid.

	//! The elements are simply stored in a list.

	typedef std::list<grid_pos> grid_t;

}

LIBCXXW_NAMESPACE_END

#endif
