/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_axisrange_H
#define metrics_grid_axisrange_H

#include <x/number.H>
#include "metrics_grid_xy.H"
#include "metrics_grid_axisrangefwd.H"

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! An element's position in a grid, on the X or the Y axis.

	class LIBCXX_HIDDEN grid_axisrange {

	public:

		//! The starting X or Y position.

		grid_xy start;

		//! The ending X or Y position.

		grid_xy end;

		//! Constructor

		grid_axisrange();

		//! Destructor

		~grid_axisrange();

		//! Make a sanity check
		void validate() const;
	};
}

LIBCXXW_NAMESPACE_END

#endif
