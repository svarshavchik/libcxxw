/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridlayoutmanager_impl_elements_h
#define gridlayoutmanager_impl_elements_h

#include "gridlayoutmanager.H"
#include "metrics_grid.H"
#include "x/w/impl/element.H"
#include "straight_borderfwd.H"
#include "corner_borderfwd.H"
#include "synchronized_axis_valueobj.H"
#include "metrics_grid_xy.H"
#include <vector>
#include <tuple>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! grid element calculation data.

//! Object encapsulates all data needed to calculate grid elements'
//! positions.
//!
//! This is an ONLY IN_THREAD object.

class gridlayoutmanagerObj::implObj::elementsObj
	: public synchronized_axis_valueObj {

	//! My container element.

	const container_impl my_container;
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
		bool takes_up_space;
		halign horizontal_alignment;
		valign vertical_alignment;

		pos_axis(const metrics::grid_pos &pos,
			 bool takes_up_space,
			 const metrics::horizvert &axises,
			 const metrics::pos_axis_padding &padding,
			 const element &child_element,
			 halign horizontal_alignment,
			 valign vertical_alignment)
			: metrics::pos_axis({pos, axises, padding}),
			child_element(child_element),
			takes_up_space(takes_up_space),
			horizontal_alignment(horizontal_alignment),
			vertical_alignment(vertical_alignment) {}
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

	straight_border get_straight_border(ONLY IN_THREAD,
					    const container_impl
					    &container_impl,
					    straight_border_factory_t factory,
					    straight_border_update_t update,

					    const current_border_implptr
					    &default_border,

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

	corner_border get_corner_border(ONLY IN_THREAD,
					const container_impl
					&container_impl,
					metrics::grid_xy x,
					metrics::grid_xy y);

	//! Register a bor
	elementsObj(const container_impl &);

	~elementsObj();

	/////////////////////////////////////////////////////////////////////
	//
	// Metrics calculation data.

	//! Cached metrics for all columns.

	metrics::grid_metrics_t unsynchronized_horiz_metrics;

	//! Whether this grid is synchronized with another grid.
	bool do_synchronization=false;

	//! Synchronized metrics with other grids, before they're scaled.

	//! If we're not synchronizing this is the same as
	//! unsynchronized_horiz_metrics, otherwise this is copied from
	//! synchronized_columns.
	metrics::grid_metrics_t unscaled_horiz_metrics;

	//! Scaled synchronized metrics with other grids.

	//! If we're not synchronizing this is the same as
	//! unsynchronized_horiz_metrics, otherwise this is copied from
	//! synchronized_columns.
	metrics::grid_metrics_t scaled_horiz_metrics;

	//! Cached metrics for all rows.

	metrics::grid_metrics_t vert_metrics;

	//! Calculate total horiz_metrics or vert_metrics

	metrics::axis total_metrics(const metrics::grid_metrics_t &metrics);

	//! Recalculate horiz_metrics and vert_metrics

	//! The bool parameter specifies whether the recalculation was
	//! triggered by addition or removal of elements from the grid.
	//!
	//! Computes total metrics. Returns:
	//!
	//! - bool flag: true if the flag parameter was true, or the total
	//! metrics have changed.
	//!
	//! - new horizontal and vertical metrics.

	std::tuple<bool, metrics::axis, metrics::axis>
	recalculate_metrics(ONLY IN_THREAD,
			    grid_map_t::lock &lock,
			    my_synchronized_axis &synchronized_columns,
			    bool flag);

	//! Implement synchronized_axis_updated

	//! Triggers recalculation.
	void synchronized_axis_updated(ONLY IN_THREAD,
				       const synchronized_axis_values_t &)
		override;

	//! Update scaled_horiz_metrics

	//! From the synchronized axis.
	bool update_scaled_metrics(ONLY IN_THREAD,
				   const synchronized_axis_values_t &);

	//! Cached sizes for all columns

	metrics::grid_sizes_t horiz_sizes;

	//! Cached sizes for all rows

	metrics::grid_sizes_t vert_sizes;

	//! Recalculate horiz_sizes and vert_sizes

	bool recalculate_sizes(ONLY IN_THREAD,
			       const grid_map_t::lock &lock,
			       my_synchronized_axis &synchronized_columns,
			       dim_t target_width,
			       dim_t target_height);

	//! Now that everything's calculate, compute an element's position

	rectangle compute_element_position(ONLY IN_THREAD,
					   const metrics::grid_pos &child,
					   const element_impl &e_impl);
};

LIBCXXW_NAMESPACE_END

#endif
