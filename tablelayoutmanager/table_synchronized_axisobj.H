/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_synchronized_axisobj_h
#define x_w_table_synchronized_axisobj_h

#include "synchronized_axis_impl.H"
#include "tablelayoutmanager/table_synchronized_axisfwd.H"
#include "includes/x/w/tablelayoutmanager.H"
#include "includes/x/w/impl/connection_threadfwd.H"

#include <x/mpobj.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see table_synchronized_axis

class LIBCXX_HIDDEN tablelayoutmanagerObj::table_synchronized_axisObj
	: public synchronized_axisObj::implObj {

public:

	//! A thread-safe container for dragged columns values.

	//! This is what's safe, requiring this to be thread-safe.
	typedef mpobj<std::optional<std::vector<metrics::axis>
				    >,
		      std::recursive_mutex> dragged_scaled_axis_t;

	//! Columns have been dragged.

	//! Once columns are dragged, scaled_derived_axis() returns this.

	dragged_scaled_axis_t dragged_scaled_axis;

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
	//! Previously saved, now restored columns width

	std::vector<dim_t> restored_widths;

 public:

	//! Obtain a reference to restored_width

	//! This is used just after this object is constructed to obtain
	//! a reference to restored_widths, for the purpose of, well, restoring
	//! them. This is never used again.

	std::vector<dim_t> &get_restored_widths()
	{
		return restored_widths;
	}

 private:
	//! Cached coordinates and sizes of border columns only

	//! Cached for lookup_draggable_border()'s speed.
	std::vector<std::tuple<coord_t, dim_t>> border_positions_thread_only;

	//! Update border_positions.

	//! Recomputed the cached border positions.
	void update_border_positions(ONLY IN_THREAD,
				     std::vector<metrics::axis> &scaled);

	struct adjusting_t {

		coord_t initial_x_coordinate;

		metrics::axis first_adjusted_metric;

		metrics::axis second_adjusted_metric;
	};

	std::optional<adjusting_t> adjusting_thread_only;

 public:
	THREAD_DATA_ONLY(border_positions);
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

	//! This is inokved when adjusting columns using the keybord.
	void adjust_to_left(ONLY IN_THREAD,
			    dim_t distance,
			    size_t first_adjusted_column,
			    size_t second_adjusted_column);

	//! This is inokved when adjusting columns using the keybord.
	void adjust_to_right(ONLY IN_THREAD,
			     dim_t distance,
			     size_t first_adjusted_column,
			     size_t second_adjusted_column);

	//! Return the synchronized axi to unadjusted values.
	void clear_adjustments(ONLY IN_THREAD);

	//! Return the coordinates of the border to the right of the column

	std::optional<std::tuple<coord_t, dim_t>
		      > border_center(ONLY IN_THREAD,
				      size_t first_adjusted_column);

 private:
	//! Something unexpected happen and we want to stop dragging.

	//! If it's in progress.
	void abort_dragging(ONLY IN_THREAD,
			    dragged_scaled_axis_t::lock &lock);

	//! Closure used by adjust_common

	typedef std::tuple<dim_t, dim_t>
		adjust_callback(dim_t, dim_t, dim_t, dim_t);

	//! Shared logic by adjust(), adjust_to_left(), adjust_to_right().

	template<typename functor>
		void adjust_common(ONLY IN_THREAD,
				   size_t first_adjusted_column,
				   size_t second_adjusted_column,
				   functor &&f)
	{
		do_adjust_common(IN_THREAD,
				 first_adjusted_column,
				 second_adjusted_column,
				 make_function<adjust_callback>
				 (std::forward<functor>(f)));
	}

	//! Type-erased adjust_common()

	void do_adjust_common(ONLY IN_THREAD,
			      size_t first_adjusted_column,
			      size_t second_adjusted_column,
			      const function<adjust_callback> &);

 public:
	//! It appears that we've been resized after dragging.

	void resize_dragged_scaled_axis(ONLY IN_THREAD,
					dragged_scaled_axis_t::lock
					&scaled_lock,
					dim_squared_t
					total_scaled_except_borders_width,
					dim_squared_t
					total_dragged_except_borders_width);

	//! A container of reference sizes, for resizing the table.

	//! The first time the table gets resized, the pre-resize
	//! dragged_scaled_axis becomes the reference_axis_for_resizing
	//! and the resized axis get computed each time by scaling
	//! the same reference_axis_for_sizing.
	//!
	//! This avoids rounding errors from accumulating as a result of
	//! repeatedly scaling the previously-scaled values.
	//!
	//! Manually adjust()ing the axis drops the reference info. No longer
	//! valid.

	struct resize_reference_info_t {

		//! Original dragged_scaled_axis values.
		std::vector<metrics::axis> reference_axis_for_sizing;

		//! The sum total of all non-border reference_axis columns

		//! Cached for speed. Only the non-border columns get scaled.
		dim_squared_t reference_axis_except_borders_width;
	};

	//! Reference sizes when resizing the table.
	std::optional<resize_reference_info_t> resize_reference_info;
};

LIBCXXW_NAMESPACE_END

#endif
