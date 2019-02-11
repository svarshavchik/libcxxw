/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutmanager_menuitem_h
#define listlayoutmanager_menuitem_h

#include "x/w/impl/popup/popupfwd.H"
#include "x/w/impl/popup/popupobj.H"
#include <variant>

LIBCXXW_NAMESPACE_START

//! This menu item is an option

//! \see menu_item_t

struct LIBCXX_HIDDEN menu_item_option {};

//! This menu item is a submenu

struct LIBCXX_HIDDEN menu_item_submenu {
	popup                           submenu_popup;
	ref<popupObj::handlerObj>	submenu_popup_handler;
};

//! Menu item type.

//! This is used by \ref listlayoutmanager "list layout managers" for a
//! menu. This is used when constructing the menu item, and specifies what
//! kind of a menu item this is:
//!
//! - the default constructor specifies an ordinary menu item, that invokes
//! its callback, and not much more.
//!
//! - menu_item_option: this menu item is an option. Besides invoking the
//! callback, a bullet is added or removed next to the menu item, indicating
//! whether this option is selected or not.
//!
//! - menu_item_submenu: this menu item has a submenu, and this specifies
//! the sub-menu popup.

typedef std::variant<std::monostate, menu_item_option, menu_item_submenu
		     > menu_item_t;

LIBCXXW_NAMESPACE_END

#endif
