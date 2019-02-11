/*
** Copyright 2017-2019 Double Precision, Inc.
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

LIBCXXW_NAMESPACE_START

menubar_hotspot_implObj
::menubar_hotspot_implObj(const popup &menu_popup,
			  const ref<popupObj::handlerObj> &menu_popup_handler,
			  const background_color &bg_color,
			  const background_color &highlighted_color,
			  const background_color &clicked_color,
			  const container_impl &container_impl)
	: superclass_t{menu_popup_handler,
		       bg_color, highlighted_color, clicked_color,
		       container_impl},
	  menu_popup{menu_popup}
{
}

menubar_hotspot_implObj::~menubar_hotspot_implObj()=default;

bool menubar_hotspot_implObj::focus_autorestorable(ONLY IN_THREAD)
{
	return false;
}

LIBCXXW_NAMESPACE_END
