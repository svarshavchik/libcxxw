/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/focus_border_appearance.H"
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
	auto &c=bc.get_container_impl();
	auto &e=c.container_element_impl();

	const auto &appearance=get_appearance();

	const auto &b=e.current_keyboard_focus(IN_THREAD)
		? appearance->focuson_border
		: appearance->focusoff_border;

	if (bc.do_set_border(IN_THREAD, b, b, b, b))
	{
		c.invoke_layoutmanager
			([&]
			 (const auto &impl)
			 {
				 impl->needs_recalculation(IN_THREAD);
			 });
	}
}

LIBCXXW_NAMESPACE_END
