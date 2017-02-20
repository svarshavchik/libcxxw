/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_impl_elements_h
#define gridlayoutmanager_impl_elements_h

#include "gridlayoutmanager.H"
#include "metrics_grid.H"
#include "element.H"
#include "straight_borderfwd.H"
#include "corner_borderfwd.H"
#include "metrics_grid_xy.H"
#include <vector>
#include <tuple>
#include <unordered_map>

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

	//! Flag, a child metrics has been updated.

	bool child_metrics_updated_flag=false;

	//! A cached vector of all elements in the grid.

	std::vector<pos_axis> all_elements;

	//! Metadata stored in \c straight_border_info

	struct straight_border_info {

		//! The border
		straight_border border;

		//! The border's position.
		metrics::grid_pos pos;

		//! Flag used when rebuilding the straight borders.
		bool is_current;
	};

	//! Metadata stored in \c corner_border_info

	struct corner_border_info {

		//! The border
		corner_border border;

		//! The border's position.
		metrics::grid_pos pos;

		//! Flag used when rebuilding the straight borders.
		bool is_new;

		//! Flag used when rebuilding the straight borders.
		bool is_updated;
	};

	//! Hash straight border starting positions, and corner border positions.

	struct border_map_hash  {

		size_t operator()(const std::tuple<metrics::grid_xy,
				  metrics::grid_xy> &xy) const;
	};

	//! Current straight borders.

	//! Based on calculate_borders(), this container stores the
	//! straight_border elements. The container keeps track of
	//! both the horizontal and vertical borders, since their starting
	//! position is unambiguous (vertical borders always are on the even
	//! x coordinates, while horizontal borders start and end on odd
	//! x coordinates).
	//!
	//! calculate_borders() computes each border's x/y coordinate, and
	//! its size. From that, straight_border_info gets constructed, and
	//! stored in this map.
	//!
	//! The map's key is the starting and ending x/y coordinate. When
	//! rebuilding the straight border map if we find an existing
	//! entry that matches the coordinates, we'll update_horizontal_border()
	//! or update_vertical_border(), as needed, as an optimization.
	//!
	//! Before rebuilding all borders, every existing border's
	//! is_current flag gets cleared, and its get set as an existing
	//! border is updated, or a new border gets created, afterwards,
	//! all remaining borders with is_current not set are removed, they're
	//! obsolete.

	std::unordered_map< std::tuple<metrics::grid_xy,
				       metrics::grid_xy>,
			    straight_border_info,
			    border_map_hash> straight_borders;

	straight_border get_straight_border(IN_THREAD_ONLY,
					    const ref<containerObj::implObj>
					    &container_impl,
					    straight_border_factory_t factory,
					    straight_border_update_t update,

					    metrics::grid_xy xstart,
					    metrics::grid_xy xend,
					    metrics::grid_xy ystart,
					    metrics::grid_xy yend,

					    const grid_elementptr &e1,
					    const grid_elementptr &e2);

	//! Current corner borders

	//! After the straight borders are calculated, their information
	//! gets used to build the corner border elements.

	std::unordered_map< std::tuple<metrics::grid_xy,
				       metrics::grid_xy>,
			    corner_border_info,
			    border_map_hash> corner_borders;

	//! Create a corner border cell, if needed.

	corner_border get_corner_border(IN_THREAD_ONLY,
					const ref<containerObj::implObj>
					&container_impl,
					metrics::grid_xy x,
					metrics::grid_xy y);

	//! Register a bor
	elementsObj();

	~elementsObj();

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
