/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframecontainer_impl.H"
#include "focus/focusframecontainer_element.H"
#include "gridlayoutmanager.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

focusframecontainerObj::implObj::implObj()=default;

focusframecontainerObj::implObj::~implObj()=default;

void focusframecontainerObj::implObj
::keyboard_focus(IN_THREAD_ONLY,
		 focus_change event,
		 const ref<elementObj::implObj> &ptr)
{
	update_focusframe(IN_THREAD);
}

void focusframecontainerObj::implObj
::window_focus_change(IN_THREAD_ONLY, bool flag)
{
	update_focusframe(IN_THREAD);
}

void focusframecontainerObj::implObj
::update_focusframe(IN_THREAD_ONLY)
{
	get_container_impl().invoke_layoutmanager
		([&]
		 (const ref<gridlayoutmanagerObj::implObj> &manager)
		 {
			 gridlayoutmanagerObj::grid_map_t::lock
				 lock{manager->grid_map};

			 lock->elements_have_been_modified();
			 manager->needs_recalculation(IN_THREAD);
		 });
}

LIBCXXW_NAMESPACE_END
