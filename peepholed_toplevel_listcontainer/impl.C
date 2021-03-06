/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"
#include "peepholed_toplevel_listcontainer/impl.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

peepholed_toplevel_listcontainer_implObj
::peepholed_toplevel_listcontainer_implObj()=default;

peepholed_toplevel_listcontainer_implObj
::~peepholed_toplevel_listcontainer_implObj()=default;

bool peepholed_toplevel_listcontainer_implObj
::update_tallest_row_height(ONLY IN_THREAD,
			    const tallest_row_height_t &new_tallest_height)
{
	if (tallest_row_height(IN_THREAD) == new_tallest_height)
		return false;

	tallest_row_height(IN_THREAD)=new_tallest_height;

	// The popup's vertical peephole scrollbar's increment is based on
	// talltest_row_height, so we need to trigger our parent container's
	// to recalculate, so that the toplevelpeephole_layoutmanagerObj's
	// recalculate() can update the vertical increment.

	listcontainer_element().child_container->needs_recalculation(IN_THREAD);
	return true;
}


LIBCXXW_NAMESPACE_END
