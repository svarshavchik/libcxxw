/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_xy_H
#define metrics_grid_xy_H

#include <x/number.H>
#include "x/w/namespace.H"
#include <cstdint>

LIBCXXW_NAMESPACE_START

namespace metrics {

	class grid_xy_tag;

	//! This numeric type represents a display element's X or Y grid position

	typedef number<uint16_t, grid_xy_tag> grid_xy;


	//! Specified axis size, as n%
	typedef unsigned axis_size_t;

}

LIBCXXW_NAMESPACE_END

#endif
