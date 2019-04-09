/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "tablelayoutmanager/table_synchronized_axis.H"
#include "tablelayoutmanager/tablelayoutmanager_impl.H"
#include "calculate_borders.H"
#include "screen_positions_impl.H"
#include "catch_exceptions.H"
#include <x/exception.H>
#include <algorithm>

LIBCXXW_NAMESPACE_START

tablelayoutmanagerObj::table_synchronized_axisObj
::table_synchronized_axisObj(const new_tablelayoutmanager &ntlm)
	: adjustable_column_widths{ntlm.adjustable_column_widths},
	  restored_widths{ntlm.restored_widths}
{
}

tablelayoutmanagerObj::table_synchronized_axisObj::~table_synchronized_axisObj()=default;

// After individual column widths were adjusted, we keep an eye on the
// total_scaled_width, which is the original width of all the columns before
// they are overridden by the adjusted width.
//
// If they differ it means that the display element must've been resized,
// so we need to figure out how to adjust the individual columns ourselves.
//
// The variables:
//
// - total_scaled_width: the presumed new width of the table
//
// - total_dragged_width: what we remember the total width of the table was
//
// - ..except_borders: the same, but without counting the fixed-size border
// columns.
//
// Factored out of scale_derived_values(), for readability.

inline void tablelayoutmanagerObj::table_synchronized_axisObj
::resize_dragged_scaled_axis(ONLY IN_THREAD,
			     dragged_scaled_axis_t::lock &scaled_lock,
			     dim_squared_t
			     total_scaled_except_borders_width,
			     dim_squared_t
			     total_dragged_except_borders_width)
{
	auto &scaled=**scaled_lock;
	// If this is the first such resize, capture the current numbers
	// as a reference.

	if (!resize_reference_info)
	{
		resize_reference_info=
			{
			 scaled,
			 total_dragged_except_borders_width,
			};
	}
	auto &info=*resize_reference_info;

	size_t i=info.reference_axis_for_sizing.size();
	if (i != scaled.size() ||
	    info.reference_axis_except_borders_width == 0)
	{
		abort_dragging(IN_THREAD, scaled_lock); // Shouldn't happen
		return;
	}
	dim_squared_t numerator=0;

	while (i)
	{
		--i;

		if (IS_BORDER_RESERVED_COORD(i))
			continue;

		numerator += info.reference_axis_for_sizing[i].minimum()
			* total_scaled_except_borders_width;

		dim_t n=dim_t::truncate(numerator /
					info.reference_axis_except_borders_width
					);

		numerator=dim_t::truncate
			(numerator % info.reference_axis_except_borders_width);

		scaled[i]={n,n,n};
	}
	update_border_positions(IN_THREAD, scaled);
}

std::vector<metrics::axis>
tablelayoutmanagerObj::table_synchronized_axisObj
::scale_derived_values(ONLY IN_THREAD,
		       const synchronized_axis_values_t &values,
		       const std::vector<metrics::derivedaxis> &derived_values)
{
	auto scaled=synchronized_axisObj::implObj
		::scale_derived_values(IN_THREAD,
				       values,
				       derived_values);

	if (!adjustable_column_widths)
		return scaled; // Avoid this work.

	dragged_scaled_axis_t::lock scaled_lock{dragged_scaled_axis};

	if (scaled.size() == restored_widths.size())
	{
		// If we restored previously-saved widths, now is the time
		// to put them back, here.

		scaled_lock->emplace();
		auto &v=**scaled_lock;
		v.clear();
		v.reserve(scaled.size());

		for (const auto &w:restored_widths)
			v.emplace_back(w, w, w);

		restored_widths.clear();
	}

	if (*scaled_lock)
	{
		auto &v=**scaled_lock;

		// We do not expect any changes in # of columns, or
		// metrics of borders. If there are, call the show off!

		if (v.size() != scaled.size())
			abort_dragging(IN_THREAD, scaled_lock);
		else
		{
			dim_squared_t total_scaled_width=0;

			dim_squared_t total_scaled_except_borders_width=0;

			dim_squared_t total_dragged_width=0;

			dim_squared_t total_dragged_except_borders_width=0;

			bool changed=false;

			for (size_t i=scaled.size(); i; )
			{
				--i;

				total_scaled_width += scaled[i].minimum();
				total_dragged_width += v[i].minimum();

				if (!IS_BORDER_RESERVED_COORD(i))
				{
					total_scaled_except_borders_width +=
						scaled[i].minimum();
					total_dragged_except_borders_width +=
						v[i].minimum();
					continue;
				}

				// The border metrics can change due to a theme
				// update, so we must update them as well.
				if (v[i] != scaled[i])
				{
					changed=true;
					v[i]=scaled[i];
				}
			}

			if (changed ||
			    total_scaled_width != total_dragged_width)
			{
				resize_dragged_scaled_axis
					(IN_THREAD,
					 scaled_lock,
					 total_scaled_except_borders_width,
					 total_dragged_except_borders_width);
			}
		}
	}

	if (*scaled_lock)
	{
		scaled=**scaled_lock;
		return scaled;
	}

	adjusting(IN_THREAD).reset(); // Shouldn't be, if was, no good any more

	update_border_positions(IN_THREAD, scaled);
	return scaled;
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::update_border_positions(ONLY IN_THREAD,
			  std::vector<metrics::axis> &scaled)
{
	// Cache where the borders are.

	border_positions_thread_only.clear();
	border_positions_thread_only
		.reserve(NONBORDER_COORD_TO_ROWCOL(scaled.size()+1));

	coord_t x=0;
	size_t i=0;

	for (auto &axis:scaled)
	{
		auto m=axis.minimum();
		if (IS_BORDER_RESERVED_COORD(i))
		{
			border_positions(IN_THREAD).emplace_back(x, m);
		}

		// Take this opportunity to fixate border columns' metrics

		// This is called with the value returned from the original
		// scaled_derived_values(). All border elements should be
		// fixed metrics, but we'll make sure, here.

		axis={m, m, m};
		++i;
		x=coord_t::truncate(x+m);
	}
}

size_t tablelayoutmanagerObj::table_synchronized_axisObj
::lookup_draggable_border(ONLY IN_THREAD, coord_t x,
			  dim_t buffer) const
{
	auto b=border_positions(IN_THREAD).begin(),
		e=border_positions(IN_THREAD).end();

	auto found=std::upper_bound(b, e, x,
				    []
				    (coord_t value, const auto &e)
				    {
					    return value < std::get<0>(e);
				    });

	if (found == b)
		return 0; // Before the first border.

	const auto &[border_x, border_w]=found[-1];

	coord_t end_x{coord_t::truncate(border_x+(border_w < buffer
						  ? buffer:border_w))};

	// Column 0 is the leftmost border which is not draggable.
	// If we return it, that's fine.

	if (end_x > x)
	{
		// Pointer is at this border #.
		size_t border_index=(found-b-1);

		if (border_index == 0)
			return 0; // We don't scroll this border.

		return CALCULATE_BORDERS_COORD(border_index-1);
	}

	if (found < e)
	{
		// Maybe the pointer is just before this border.

		coord_t end_x=coord_t::truncate(x+buffer);

		if (end_x > std::get<0>(*found))
		{
			size_t border_index=(found-b-1);

			return CALCULATE_BORDERS_COORD(border_index);
		}
	}

	return 0;
}

std::optional<std::tuple<coord_t, dim_t>
	      > tablelayoutmanagerObj::table_synchronized_axisObj
::border_center(ONLY IN_THREAD, size_t n)
{
	n=NONBORDER_COORD_TO_ROWCOL(n)+1;

	if (n >= border_positions(IN_THREAD).size())
		return std::nullopt;

	return border_positions(IN_THREAD)[n];
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::start_adjusting_from(ONLY IN_THREAD,
		       coord_t initial_x_coordinate,
		       size_t first_adjusted_column,
		       size_t second_adjusted_column)
{
	dragged_scaled_axis_t::lock scaled_lock{dragged_scaled_axis};

	if (!*scaled_lock)
	{
		*scaled_lock=synchronized_values::lock{values}->scaled_values;
	}

	auto &dragged_scaled_axis_values=**scaled_lock;

	size_t s=dragged_scaled_axis_values.size();

	if (first_adjusted_column >= s ||
	    second_adjusted_column >= s)
		return; // Sanity check.

	adjusting(IN_THREAD)=
		{
		 initial_x_coordinate,
		 dragged_scaled_axis_values[first_adjusted_column],
		 dragged_scaled_axis_values[second_adjusted_column]
		};
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::adjust(ONLY IN_THREAD,
	 coord_t x_coordinate,
	 size_t first_adjusted_column,
	 size_t second_adjusted_column)
{
	adjust_common
		(IN_THREAD,
		 first_adjusted_column,
		 second_adjusted_column,
		 [&]
		 (dim_t first, dim_t second, dim_t, dim_t)
		 {
			 // Compare the current x coordinate with the
			 // starting x coordinate,
			 // and then compute the new widths of the
			 // adjusted columns.

			 if (x_coordinate <
			     adjusting(IN_THREAD)->initial_x_coordinate)
			 {
				 dim_t diff=dim_t::truncate
					 (adjusting(IN_THREAD)
					  ->initial_x_coordinate-x_coordinate);

				 if (diff >= first)
				 {
					 diff=first;
					 --diff;
				 }

				 first -= diff;
				 second = dim_t::truncate(second + diff);
			 }
			 else
			 {
				 dim_t diff=dim_t::truncate
					 (x_coordinate-adjusting(IN_THREAD)
					  ->initial_x_coordinate);

				 if (diff >= second)
				 {
					 diff=second;
					 --diff;
				 }

				 second -= diff;
				 first = dim_t::truncate(first + diff);
			 }

			 return std::tuple{first, second};
		 });
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::adjust_to_left(ONLY IN_THREAD,
		 dim_t distance,
		 size_t first_adjusted_column,
		 size_t second_adjusted_column)
{
	adjust_common
		(IN_THREAD,
		 first_adjusted_column,
		 second_adjusted_column,
		 [&]
		 (dim_t, dim_t, dim_t first, dim_t second)
		 {
			 if (distance >= first)
			 {
				 distance=first;
				 --distance;
			 }
			 first -= distance;
			 second = dim_t::truncate(second + distance);

			 return std::tuple{first, second};
		 });
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::adjust_to_right(ONLY IN_THREAD,
		  dim_t distance,
		  size_t first_adjusted_column,
		  size_t second_adjusted_column)
{
	adjust_common
		(IN_THREAD,
		 first_adjusted_column,
		 second_adjusted_column,
		 [&]
		 (dim_t, dim_t, dim_t first, dim_t second)
		 {
			 if (distance >= second)
			 {
				 distance=second;
				 --distance;
			 }
			 second -= distance;
			 first = dim_t::truncate(first + distance);

			 return std::tuple{first, second};
		 });
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::do_adjust_common(ONLY IN_THREAD,
		   size_t first_adjusted_column,
		   size_t second_adjusted_column,
		   const function<adjust_callback> &update)
{
	dragged_scaled_axis_t::lock scaled_lock{dragged_scaled_axis};

	// Sanity check.

	if (!*scaled_lock || !adjusting(IN_THREAD))
		return;

	// Get rid of any reference info we were using for resizing
	resize_reference_info.reset();

	auto &dragged_scaled_axis_values=**scaled_lock;

	size_t s=dragged_scaled_axis_values.size();

	if (first_adjusted_column >= s ||
	    second_adjusted_column >= s)
		return;

	auto [first, second]=
		update(adjusting(IN_THREAD)->first_adjusted_metric.minimum(),
		       adjusting(IN_THREAD)->second_adjusted_metric.minimum(),
		       dragged_scaled_axis_values[first_adjusted_column]
		       .minimum(),
		       dragged_scaled_axis_values[second_adjusted_column]
		       .minimum());

	if (first ==
	    dragged_scaled_axis_values[first_adjusted_column].minimum() &&
	    second ==
	    dragged_scaled_axis_values[second_adjusted_column].minimum())
		return;

	dragged_scaled_axis_values[first_adjusted_column]={first, first, first};
	dragged_scaled_axis_values[second_adjusted_column]=
		{second, second, second};

	// Update our cached border positions, and the synchronized axis values,
	// then notify everyone.
	update_border_positions(IN_THREAD, dragged_scaled_axis_values);

	synchronized_values::lock lock{values};

	if (first_adjusted_column >= lock->scaled_values.size() ||
	    second_adjusted_column >= lock->scaled_values.size())
		return; // Sanity check.

	lock->scaled_values[first_adjusted_column]=
		dragged_scaled_axis_values[first_adjusted_column];
	lock->scaled_values[second_adjusted_column]=
		dragged_scaled_axis_values[second_adjusted_column];
	lock->notify(IN_THREAD);
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::clear_adjustments(ONLY IN_THREAD)
{
	dragged_scaled_axis_t::lock scaled_lock{dragged_scaled_axis};

	abort_dragging(IN_THREAD, scaled_lock);
}

void tablelayoutmanagerObj::table_synchronized_axisObj
::abort_dragging(ONLY IN_THREAD,
		 dragged_scaled_axis_t::lock &lock)
{
	lock->reset();
	resize_reference_info.reset();

	{
		synchronized_values::lock lock{values};

		lock->recalculate(IN_THREAD, lock->all_values.end());
	}
}


void tablelayoutmanagerObj::save(const std::string &name,
				 const const_screen_positions &pos) const
{
	auto writelock=pos->impl->create_writelock_for_saving("table", name);

	table_synchronized_axisObj::dragged_scaled_axis_t::lock lock
		{
		 impl->axis_impl->dragged_scaled_axis
		};

	if (!*lock)
	{
		writelock->remove();
		return;
	}

	for (const auto &m:**lock)
	{
		std::ostringstream o;

		o << m.minimum();

		writelock->create_child()->element({"width"})->text(o.str())
			->parent()->parent();
	}
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::new_tablelayoutmanager_restore,
		    restore_log);

void new_tablelayoutmanager::restore(const const_screen_positions &pos,
				     const std::string_view &name)
{
	LOG_FUNC_SCOPE(restore_log);

	auto lock=pos->impl->data->readlock();

	if (!lock->get_root())
	    return;

	auto xpath=lock->get_xpath(saved_element_to_xpath("table", name));

	if (xpath->count() != 1)
		return;
	xpath->to_node();

	xpath=lock->get_xpath("width");

	size_t n=xpath->count();

	restored_widths.clear();
	restored_widths.reserve(n);

	try {
		try {
			for (size_t i=1; i <= n; ++i)
			{
				xpath->to_node(i);

				std::istringstream w{lock->get_text()};

				dim_t n;

				if (!(w >> n))
					throw EXCEPTION("Invalid saved value.");

				restored_widths.push_back(n);
			}
			return;
		} catch (const exception &e)
		{
			std::stringstream o;

			o << "Error restoring table \""
			  << name << "\": " << e;

			throw EXCEPTION(e);
		}
	} CATCH_EXCEPTIONS;

	restored_widths.clear();
}
LIBCXXW_NAMESPACE_END
