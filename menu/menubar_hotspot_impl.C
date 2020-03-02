/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menubar_hotspot_implobj.H"
#include "hotspot_bgcolor_element.H"
#include "x/w/impl/container_element.H"
#include "popup/popup.H"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "popup/popup_showhide_element.H"
#include "generic_window_handler.H"
#include "shared_handler_data.H"

LIBCXXW_NAMESPACE_START

menubar_hotspot_implObj
::menubar_hotspot_implObj(const popup &menu_popup,
			  const ref<popupObj::handlerObj> &menu_popup_handler,
			  const color_arg &highlighted_color,
			  const color_arg &clicked_color,
			  const container_impl &container_impl)
	: superclass_t{menu_popup_handler,
		       highlighted_color, clicked_color,
		       container_impl},
	  menu_popup{menu_popup}
{
}

menubar_hotspot_implObj::~menubar_hotspot_implObj()=default;

bool menubar_hotspot_implObj::focus_autorestorable(ONLY IN_THREAD)
{
	return false;
}

void menubar_hotspot_implObj::pointer_focus(ONLY IN_THREAD,
					    const callback_trigger_t &trigger)
{
	superclass_t::pointer_focus(IN_THREAD, trigger);

	auto status=current_pointer_focus(IN_THREAD);

	if (status == previous_pointer_focus)
		return;

	previous_pointer_focus=status;

	if (status) // Pointer entered the hotspot
	{
		elementObj::implObj &me=*this;

		auto &wh=me.get_window_handler();

		// If another menu is open, but not us, and the pointer
		// just entered here, we'll close the other menu and open
		// this one.

		if (!my_popup_handler->data(IN_THREAD).requested_visibility &&
		    wh.handler_data->any_menu_popups_opened(IN_THREAD))
		{
			focusableObj::implObj *i_am_focusable=this;

			i_am_focusable->request_focus_if_possible(IN_THREAD,
								  true);
			my_popup_handler->request_visibility(IN_THREAD, true);
		}
	}
}

LIBCXXW_NAMESPACE_END
