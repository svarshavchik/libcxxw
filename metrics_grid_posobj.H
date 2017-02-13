/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_grid_grid_posobj_H
#define metrics_grid_grid_posobj_H

#include "metrics_grid_axisrange.H"

#include <x/obj.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! An element in a grid: its horizontal and vertical position, metrics.

	//! \see grid_pos

	class LIBCXX_HIDDEN grid_posObj : virtual public obj {
	public:
		//! Element's horizontal position
		grid_axisrange horiz_pos;

		//! Element's vertical position
		grid_axisrange vert_pos;

		//! Constructor
		grid_posObj();

		//! Destructor
		~grid_posObj();

		//! Make a sanity check
		void validate() const;
	};
}

LIBCXXW_NAMESPACE_END

#endif
