/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "tablelayoutmanager/table_synchronized_axis.H"
#include "calculate_borders.H"
#include <algorithm>

LIBCXXW_NAMESPACE_START

tablelayoutmanagerObj::table_synchronized_axisObj
::table_synchronized_axisObj(const new_tablelayoutmanager &ntlm)
	: adjustable_column_widths{ntlm.adjustable_column_widths}
{
}

tablelayoutmanagerObj::table_synchronized_axisObj::~table_synchronized_axisObj()=default;

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

	if (dragged_scaled_axis(IN_THREAD))
	{
		auto &v=*dragged_scaled_axis(IN_THREAD);

		// We do not expect any changes in # of columns, or
		// metrics of borders. If there are, call the show off!

		if (v.size() != scaled.size())
			dragged_scaled_axis(IN_THREAD).reset();
		else
		{
			for (size_t i=scaled.size(); i; )
			{
				--i;
				if (IS_BORDER_RESERVED_COORD(i) &&
				    scaled[i].minimum() != v[i].minimum())
				{
					dragged_scaled_axis(IN_THREAD).reset();
					break;
				}
			}
		}
	}

	if (dragged_scaled_axis(IN_THREAD))
	{
		scaled=*dragged_scaled_axis(IN_THREAD);
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

void tablelayoutmanagerObj::table_synchronized_axisObj
::start_adjusting_from(ONLY IN_THREAD,
		       coord_t initial_x_coordinate,
		       size_t first_adjusted_column,
		       size_t second_adjusted_column)
{
	if (!dragged_scaled_axis(IN_THREAD))
	{
		dragged_scaled_axis(IN_THREAD)=
			synchronized_values::lock{values}->scaled_values;
	}

	auto &dragged_scaled_axis_values=*dragged_scaled_axis(IN_THREAD);

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
	// Sanity check.

	if (!dragged_scaled_axis(IN_THREAD) || !adjusting(IN_THREAD))
		return;

	auto &dragged_scaled_axis_values=*dragged_scaled_axis(IN_THREAD);

	size_t s=dragged_scaled_axis_values.size();

	if (first_adjusted_column >= s ||
	    second_adjusted_column >= s)
		return;

	dim_t first=adjusting(IN_THREAD)->first_adjusted_metric.minimum();
	dim_t second=adjusting(IN_THREAD)->second_adjusted_metric.minimum();

	// Compare the current x coordinate with the starting x coordinate,
	// and then compute the new widths of the adjusted columns.

	if (x_coordinate < adjusting(IN_THREAD)->initial_x_coordinate)
	{
		dim_t diff=dim_t::truncate
			(adjusting(IN_THREAD)->initial_x_coordinate
			 -x_coordinate);

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
			(x_coordinate
			 -adjusting(IN_THREAD)->initial_x_coordinate);

		if (diff >= second)
		{
			diff=second;
			--diff;
		}

		second -= diff;
		first = dim_t::truncate(first + diff);
	}

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

LIBCXXW_NAMESPACE_END
