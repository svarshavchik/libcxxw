/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menubar_container_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "container_element.H"
#include "hotspot_bgcolor_element.H"
#include "background_color_element.H"
#include "always_visible.H"

LIBCXXW_NAMESPACE_START

menubar_container_implObj
::menubar_container_implObj(const ref<containerObj::implObj> &parent)
	: menubar_container_superclass_impl_t(parent)
{
}

menubar_container_implObj::~menubar_container_implObj()=default;

void menubar_container_implObj::fix_order(ONLY IN_THREAD,
					  const menu &new_element)
{
	invoke_layoutmanager([&]
			     (const ref<menubarlayoutmanagerObj::implObj>
			      &manager_impl)
			     {
				     manager_impl->fix_order(IN_THREAD,
							     new_element);
			     });
}

LIBCXXW_NAMESPACE_END
