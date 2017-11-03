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
#if 0
 const ref<containerObj::implObj> &listcontainer_impl,
 #endif

 const textlist &textlist_element)
	: textlistlayoutmanagerObj::implObj(/* list */ container_impl,
					    textlist_element),
	container_impl(container_impl)
{
}

peepholed_toplevel_listcontainer_layoutmanager_implObj
::~peepholed_toplevel_listcontainer_layoutmanager_implObj()=default;

void peepholed_toplevel_listcontainer_layoutmanager_implObj
::update_tallest_row_height(IN_THREAD_ONLY, dim_t v)
{
	container_impl->update_tallest_row_height(IN_THREAD, v);
}

LIBCXXW_NAMESPACE_END
