/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager_impl_elements.H"

LIBCXXW_NAMESPACE_START

bool gridlayoutmanagerObj::implObj::rebuild_elements(IN_THREAD_ONLY)
{
	grid_map_t::lock lock(grid_map);

	if (!lock->modified)
		return false;

	std::vector<elementsObj::pos_axis> &all_elements=
		grid_elements(IN_THREAD)->all_elements;

	all_elements.clear();
	all_elements.reserve(lock->elements.size());

	for (const auto &element:lock->elements)
	{
		// We don't care about the keys. The value in the map is:
		//
		//       element - the child element.
		//       pos     - its metrics::grid_pos
		//
		// The all_elements vector contains a class that inherits from
		// metrics::pos_axis which contains:
		//       pos     - the metrics::grid_pos
		//       horizvert - the element's axises
		//
		// plus the element itself.
		//
		// So this is really just shuffling the deck, a little bit.

		all_elements.emplace_back
			(element.second.pos,
			 metrics::horizvert(element.second.element
					    ->get_horizvert(IN_THREAD)),
			 element.second.element);
	}
	lock->modified=false;

	return true;
}

bool gridlayoutmanagerObj::implObj::elementsObj
::recalculate_metrics(IN_THREAD_ONLY,
		      bool flag,
		      const metrics::horizvert &my_metrics)
{
	auto new_horiz_metrics=
		metrics::calculate_grid_horiz_metrics(all_elements);

	auto new_vert_metrics=
		metrics::calculate_grid_vert_metrics(all_elements);

	if (!flag && (horiz_metrics != new_horiz_metrics ||
		      vert_metrics != new_vert_metrics))
		flag=true;

	horiz_metrics=new_horiz_metrics;
	vert_metrics=new_vert_metrics;

	return flag;
}

bool gridlayoutmanagerObj::implObj::elementsObj
::recalculate_sizes(dim_t target_width,
		    dim_t target_height)
{
	bool flag=false;

	if (metrics::calculate_grid_size(horiz_metrics, horiz_sizes,
					 target_width))
		flag=true;

	if (metrics::calculate_grid_size(vert_metrics, vert_sizes,
					 target_height))
		flag=true;

	return flag;
}

LIBCXXW_NAMESPACE_END
