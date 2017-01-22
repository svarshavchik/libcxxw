/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_impl_elements_h
#define gridlayoutmanager_impl_elements_h

#include "gridlayoutmanager.H"
#include "metrics_grid.H"
#include "child_element.H"

#include <vector>

LIBCXXW_NAMESPACE_START

//! grid element calculation data.

//! Object encapsulates all data needed to calculate grid elements'
//! positions.
//!
//! This is an IN_THREAD_ONLY object.

class gridlayoutmanagerObj::implObj::elementsObj : virtual public obj {

public:


	//! One element in the grid.

	//! calculate_grid_horiz/vert_metrics cares only about
	//! metrics::pos_axis. Attach the actual element to it,
	//! that's what we need to work with.
	//!
	//! The grid_metrics calculation stuff doesn't care about the
	//! element objects themselves, only their metrics. But we need
	//! the object itself to work with.

	struct pos_axis : public metrics::pos_axis {

		child_element element;

		pos_axis(const metrics::grid_pos &pos,
			 const metrics::horizvert &axises,
			 const child_element &element)
			: metrics::pos_axis({pos, axises}),
			element(element) {}
	};

	//! A cached vector of all elements in the grid.

	std::vector<pos_axis> all_elements;

	elementsObj()=default;

	~elementsObj()=default;
};

LIBCXXW_NAMESPACE_END

#endif
