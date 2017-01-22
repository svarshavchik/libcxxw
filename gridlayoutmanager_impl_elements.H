/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_impl_elements_h
#define gridlayoutmanager_impl_elements_h

#include "gridlayoutmanager.H"
#include "metrics_grid.H"
#include "element.H"

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

		element child_element;

		pos_axis(const metrics::grid_pos &pos,
			 const metrics::horizvert &axises,
			 const element &child_element)
			: metrics::pos_axis({pos, axises}),
			child_element(child_element) {}
	};

	//! A cached vector of all elements in the grid.

	std::vector<pos_axis> all_elements;

	elementsObj()=default;

	~elementsObj()=default;

	/////////////////////////////////////////////////////////////////////
	//
	// Metrics calculation data.

	//! Cached metrics for all columns.

	metrics::grid_metrics_t horiz_metrics;

	//! Cached metrics for all rows.

	metrics::grid_metrics_t vert_metrics;

	//! Calculate total horiz_metrics or vert_metrics

	metrics::axis total_metrics(const metrics::grid_metrics_t &metrics);

	//! Recalculate horiz_metrics and vert_metrics

	//! The bool parameter specifies whether the recalculation was
	//! triggered by addition or removal of elements from the grid.
	//!
	//! This saves some work.

	bool recalculate_metrics(IN_THREAD_ONLY,
				 bool flag,
				 const metrics::horizvert &my_metrics);

	//! Cached sizes for all columns

	metrics::grid_sizes_t horiz_sizes;

	//! Cached sizes for all rows

	metrics::grid_sizes_t vert_sizes;

	//! Recalculate horiz_sizes and vert_sizes

	bool recalculate_sizes(dim_t target_width,
			       dim_t target_height);
};

LIBCXXW_NAMESPACE_END

#endif
