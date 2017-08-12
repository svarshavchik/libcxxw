/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "gridlayoutmanager_impl_elements.H"

LIBCXXW_NAMESPACE_START

peepholed_toplevel_listcontainer_layoutmanager_implObj
::peepholed_toplevel_listcontainer_layoutmanager_implObj
(const ref<peepholed_toplevel_listcontainer_implObj> &container_impl,
 const ref<listcontainerObj::implObj> &listcontainer_impl,
 const new_listlayoutmanager &style)
	: listlayoutmanagerObj::implObj(listcontainer_impl, style),
	container_impl(container_impl)
{
}

peepholed_toplevel_listcontainer_layoutmanager_implObj
::~peepholed_toplevel_listcontainer_layoutmanager_implObj()=default;

void peepholed_toplevel_listcontainer_layoutmanager_implObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	listlayoutmanagerObj::implObj::process_updated_position(IN_THREAD,
								position);

	dim_t tallest=0;

	for (const auto &r:grid_elements(IN_THREAD)->vert_sizes)
	{
		dim_t height=std::get<dim_t>(r.second);

		if (height > tallest)
			tallest=height;
	}

	container_impl->update_tallest_row_height(IN_THREAD, tallest);
}

LIBCXXW_NAMESPACE_END
