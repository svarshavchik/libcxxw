/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "gridlayoutmanager.H"
#include "grid_map_info.H"
#include "container_impl.H"

LIBCXXW_NAMESPACE_START

focusframecontainer_implObj::focusframecontainer_implObj()=default;

focusframecontainer_implObj::~focusframecontainer_implObj()=default;

void focusframecontainer_implObj
::keyboard_focus(ONLY IN_THREAD,
		 const callback_trigger_t &trigger)
{
	update_focusframe(IN_THREAD);
}

void focusframecontainer_implObj::window_focus_change(ONLY IN_THREAD, bool flag)
{
	update_focusframe(IN_THREAD);
}

void focusframecontainer_implObj::update_focusframe(ONLY IN_THREAD)
{
	// The focus frame gets updated by overriding
	// gridlayoutmanagerObj::focusframecontainer_implObj's rebuild_element_start(). So what
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
