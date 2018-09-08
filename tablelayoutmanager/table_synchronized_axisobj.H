/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_synchronized_axisobj_h
#define x_w_table_synchronized_axisobj_h

#include "synchronized_axis_impl.H"
#include "tablelayoutmanager/table_synchronized_axisfwd.H"
#include "includes/x/w/tablelayoutmanager.H"
#include "includes/x/w/impl/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see table_synchronized_axis

class LIBCXX_HIDDEN tablelayoutmanagerObj::table_synchronized_axisObj
	: public synchronized_axisObj::implObj {

public:
	//! Constructor
	table_synchronized_axisObj(const new_tablelayoutmanager &);

	//! Destructor
	~table_synchronized_axisObj();

	const bool adjustable_column_widths;
	//! Override scale_derived_values

	std::vector<metrics::axis>
		scale_derived_values(ONLY IN_THREAD,
				     const synchronized_axis_values_t &values,
				     const std::vector<metrics
				     ::derivedaxis> &derived_values)
		override;

 private:
	//! Cached coordinates and sizes of border columns only

	//! Cached for lookup_draggable_border()'s speed.
	std::vector<std::tuple<coord_t, dim_t>> border_positions_thread_only;

	//! Update border_positions.

	//! Recomputed the cached border positions.
	void update_border_positions(ONLY IN_THREAD,
				     std::vector<metrics::axis> &scaled);

	//! Columns have been dragged.

	//! Once columns are dragged, scaled_derived_axis() returns this.

	std::optional<std::vector<metrics::axis>
		      > dragged_scaled_axis_thread_only;

	struct adjusting_t {

		coord_t initial_x_coordinate;

		metrics::axis first_adjusted_metric;

		metrics::axis second_adjusted_metric;
	};

	std::optional<adjusting_t> adjusting_thread_only;

 public:
	THREAD_DATA_ONLY(border_positions);
	THREAD_DATA_ONLY(dragged_scaled_axis);
	THREAD_DATA_ONLY(adjusting);

	//! Is this x coordinate on top of a draggable border?

	//! Returns 0 if not. Else returns the column to the left of the border.

	size_t lookup_draggable_border(ONLY IN_THREAD, coord_t x,
				       dim_t buffer) const;

	//! Decided to adjust two columns by dragging the border between them.

	//! This is invoked when pointer button 1 is pressed.
	void start_adjusting_from(ONLY IN_THREAD,
				  coord_t initial_x_coordinate,
				  size_t first_adjusted_column,
				  size_t second_adjusted_column);

	//! This is invoked when pointer is moved with button 1 being held.
	void adjust(ONLY IN_THREAD,
		    coord_t x_coordinate,
		    size_t first_adjusted_column,
		    size_t second_adjusted_column);

	//! Something unexpected happen and we want to stop dragging.

	//! If it's in progress.
	void abort_dragging(ONLY IN_THREAD);

	//! It appears that we've been resized after dragging.

	void resize_dragged_scaled_axis(ONLY IN_THREAD,
					std::vector<metrics::axis> &scaled,
					dim_squared_t
					total_scaled_except_borders_width,
					dim_squared_t
					total_dragged_except_borders_width);

	struct resize_reference_info_t {
		std::vector<metrics::axis> reference_axis_for_sizing;
		dim_squared_t reference_axis_except_borders_width;
	};

	std::optional<resize_reference_info_t> resize_reference_info;
};

LIBCXXW_NAMESPACE_END

#endif
