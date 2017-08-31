/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menulayoutmanager_h
#define x_w_menulayoutmanager_h

#include <x/w/menulayoutmanagerfwd.H>
#include <x/w/menulayoutmanagerobj.H>
#include <x/w/shortcut.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Parameter passed to menu items' on_activated() callback.

struct menuitem_activation_info {

	//! Lock on the menu.
	menu_lock &lock;

	//! The menu layout manager
	menulayoutmanager layoutmanager;

	//! The current index of the activated item.
	size_t i;

	//! Whether the activated item has a bullet next to it.

	//! Only used when is_option is set.
	bool selected;

	//! The busy mcguffin.

	const busy &mcguffin;
};

//! This is an ordinary menu item

//! One of the possible \ref menuitem_type_t "menuitem_type_t"s.

struct menuitem_plain {

	//! Callback invoked when the menu item gets selected.
	std::function<void (const menuitem_activation_info &)> on_activate;

	//! Whether this menu item has an option bullet next to it.
	bool is_option=false;

	//! Optional shortcut for the menu item.
	shortcut menuitem_shortcut;
};

//! This is a menu item entry for a sub menu.

//! One of the possible \ref menuitem_type_t "menuitem_type_t"s.

struct menuitem_submenu {

	//! Callback invoked to create the submenu.

	std::function<void (const menulayoutmanager &)> creator;
};

LIBCXXW_NAMESPACE_END

#endif
