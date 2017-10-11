/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

peephole_gridlayoutmanagerObj
::peephole_gridlayoutmanagerObj(const ref<containerObj::implObj>
				&container_impl)
	: gridlayoutmanagerObj::implObj(container_impl)
{
	// If the peephole itself is fill-ed horizontally or vertically,
	// we want to make sure this carries over to the actual peephole
	// element, which is placed into the internal grid at (0, 0).
	// Column 0 and row 0 is set to span 100% of any extra space, and
	// the peephole element gets force-fully filled.

	requested_row_height(0, 100);
	requested_col_width(0, 100);
	col_alignment(0, halign::fill);
	row_alignment(0, valign::fill);
}

container peephole_gridlayoutmanagerObj::get_peephole_container()
{
	return get(0, 0);
}

void peephole_gridlayoutmanagerObj::request_visibility_recursive(IN_THREAD_ONLY,
								 bool flag)
{
	get_peephole_container()->elementObj::impl
		->request_visibility_recursive(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END
