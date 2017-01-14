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
#include "metrics_grid_xy.H"
#include "x/w/metrics/axis.H"

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! A container of elements arranged in a grid.

	//! The elements are simply stored in a list.

	typedef std::list<grid_pos> grid_t;

	//! Computed horizontal and vertical grid metrics.

	//! Based on the metrics given in a grid, computed metrics of
	//! rows or columns.

	typedef std::map<grid_xy, axis> grid_metrics_t;

	//! Compute a grid's horizontal metrics.

	grid_metrics_t calculate_grid_horiz_metrics(const grid_t &g)
		LIBCXX_HIDDEN;

	//! Compute a grid's vertical metrics.

	grid_metrics_t calculate_grid_vert_metrics(const grid_t &g)
		LIBCXX_HIDDEN;
}

LIBCXXW_NAMESPACE_END

#endif
