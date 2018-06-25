/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "x/w/impl/current_border_impl.H"
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
	auto &bc=focusframe_bordercontainer_impl();

	auto &e=bc.get_container_impl().container_element_impl();

	bc.set_border(IN_THREAD,
		      e.current_keyboard_focus(IN_THREAD)
		      ? get_focuson_border()
		      : get_focusoff_border());
}

LIBCXXW_NAMESPACE_END
