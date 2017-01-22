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

LIBCXXW_NAMESPACE_END
