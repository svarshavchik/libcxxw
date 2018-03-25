/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframecontainer_impl.H"
#include "focus/focusframecontainer_element.H"
#include "gridlayoutmanager.H"
#include "grid_map_info.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

focusframecontainerObj::implObj::implObj()=default;

focusframecontainerObj::implObj::~implObj()=default;

void focusframecontainerObj::implObj
::keyboard_focus(ONLY IN_THREAD,
		 const callback_trigger_t &trigger)
{
	update_focusframe(IN_THREAD);
}

void focusframecontainerObj::implObj
::window_focus_change(ONLY IN_THREAD, bool flag)
{
	update_focusframe(IN_THREAD);
}

void focusframecontainerObj::implObj
::update_focusframe(ONLY IN_THREAD)
{
	// The focus frame gets updated by overriding
	// gridlayoutmanagerObj::implObj's rebuild_element_start(). So what
	// we need to do is to pretend that the grid's elements were modified.

	get_container_impl().invoke_layoutmanager
		([&]
		 (const ref<gridlayoutmanagerObj::implObj> &manager)
		 {
			 grid_map_t::lock lock{manager->grid_map};

			 (*lock)->elements_have_been_modified();
			 manager->needs_recalculation(IN_THREAD);
		 });
}

LIBCXXW_NAMESPACE_END
