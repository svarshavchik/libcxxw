/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_layoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

menu_layoutmanager_implObj
::menu_layoutmanager_implObj(const ref<peepholed_toplevel_listcontainer_implObj
			     > &container_impl,
			     const ref<listcontainerObj::implObj>
			     &listcontainer_impl,
			     const new_listlayoutmanager &style)
	: peepholed_toplevel_listcontainer_layoutmanager_implObj
	  (container_impl, listcontainer_impl, style)
{
}

menu_layoutmanager_implObj::~menu_layoutmanager_implObj()=default;

LIBCXXW_NAMESPACE_END
