/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlistlayoutmanager_menuitem_h
#define textlistlayoutmanager_menuitem_h

#include "popup/popupfwd.H"
#include <variant>

LIBCXXW_NAMESPACE_START

// TODO: Move this into its own header:

struct LIBCXX_HIDDEN menu_item_option {};

struct LIBCXX_HIDDEN menu_item_submenu {
	popup                             submenu_popup;
	ref<popup_attachedto_handlerObj>  submenu_popup_handler;
};

typedef std::variant<std::monostate, menu_item_option, menu_item_submenu
		     > menu_item_t;

LIBCXXW_NAMESPACE_END

#endif
