/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "peephole/peephole.H"
#include "x/w/impl/container.H"
#include "x/w/scrollbar.H"

LIBCXXW_NAMESPACE_START

peephole_gridlayoutmanagerObj
::peephole_gridlayoutmanagerObj(const init_args &args)
	: peephole_gridlayoutmanagerObj{args.my_container_impl,
					args.my_peephole,
					args.my_vertical_scrollbar,
					args.my_horizontal_scrollbar}
{
}

peephole_gridlayoutmanagerObj
::peephole_gridlayoutmanagerObj(const container_impl
				&container_impl,
				const peephole &my_peephole,
				const scrollbar &my_vertical_scrollbar,
				const scrollbar &my_horizontal_scrollbar)
	: gridlayoutmanagerObj::implObj{container_impl, {}},
	  my_peephole{my_peephole},
	  my_vertical_scrollbar{my_vertical_scrollbar},
	  my_horizontal_scrollbar{my_horizontal_scrollbar}
{
	// If the peephole itself is fill-ed horizontally or vertically,
	// we want to make sure this carries over to the actual peephole
	// element, which is placed into the internal grid at (0, 0).
	// Column 0 and row 0 is set to span 100% of any extra space, and
	// the peephole element gets force-fully filled.

	grid_map_t::lock lock{grid_map};

	requested_row_height(lock, 0, 100);
	requested_col_width(lock, 0, 100);
	col_alignment(lock, 0, halign::fill);
	row_alignment(lock, 0, valign::fill);
}

void peephole_gridlayoutmanagerObj::request_visibility_recursive(ONLY IN_THREAD,
								 bool flag)
{
	my_peephole->elementObj::impl
		->request_visibility_recursive(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END
