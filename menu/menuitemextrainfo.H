/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menuitemextrainfo_h
#define menuitemextrainfo_h

#include "menu/menuitemextrainfofwd.H"
#include "menu/menuitemextrainfoobj.H"
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

LIBCXXW_NAMESPACE_END

#endif
