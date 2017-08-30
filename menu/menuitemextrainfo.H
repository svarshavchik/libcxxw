/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menuitemextrainfo_h
#define menuitemextrainfo_h

#include "menu/menuitemextrainfofwd.H"
#include "menu/menuitemextrainfoobj.H"
#include "popup/popupfwd.H"
#include "x/w/menulayoutmanager.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! A created plain menu item.

struct LIBCXX_HIDDEN created_menuitem_plain {

	//! Whether this menu item has an option bullet next to it.
	bool is_option;

	//! Callback invoked when the menu item gets selected.
	std::function<void (const menuitem_activation_info &)> on_activate;

	//! Constructor
	created_menuitem_plain(const menuitem_plain &t)
		: is_option(t.is_option),
		on_activate(t.on_activate)
		{
		}
};

//! A created submenu item.

struct LIBCXX_HIDDEN created_menuitem_submenu {

	//! Constructor
	created_menuitem_submenu(const menuitem_submenu &,
				 const popup &submenu_popup,
				 const ref<popup_attachedto_handlerObj>
				 &submenu_popup_handler);

	//! The submenu popup
	popup submenu_popup;

	//! Submenu popup's handler.
	ref<popup_attachedto_handlerObj> submenu_popup_handler;
};

LIBCXXW_NAMESPACE_END

#endif
