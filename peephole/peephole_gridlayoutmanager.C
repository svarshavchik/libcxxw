/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "peephole/peephole.H"
#include "x/w/impl/container.H"
#include "x/w/scrollbar.H"

LIBCXXW_NAMESPACE_START

peephole_gridlayoutmanagerObj
::peephole_gridlayoutmanagerObj(const container_impl
				&container_impl)
	: gridlayoutmanagerObj::implObj{container_impl, {}}
{
	// If the peephole itself is fill-ed horizontally or vertically,
	// we want to make sure this carries over to the actual peephole
	// element, which is placed into the internal grid at (0, 0).
	// Column 0 and row 0 is set to span 100% of any extra space, and
	// the peephole element gets force-fully filled.

	grid_map_t::lock grid_lock{grid_map};

	requested_row_height(grid_lock, 0, 100);
	requested_col_width(grid_lock, 0, 100);
	col_alignment(grid_lock, 0, halign::fill);
	row_alignment(grid_lock, 0, valign::fill);
}

peephole peephole_gridlayoutmanagerObj::get_peephole()
{
	return get(0, 0);
}

scrollbar peephole_gridlayoutmanagerObj::get_vertical_scrollbar()
{
	return get(0, 1);
}

scrollbar peephole_gridlayoutmanagerObj::get_horizontal_scrollbar()
{
	return get(1, 0);
}

void peephole_gridlayoutmanagerObj::request_visibility_recursive(ONLY IN_THREAD,
								 bool flag)
{
	get_peephole()->elementObj::impl
		->request_visibility_recursive(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END
