/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutstyle_implfwd_h
#define x_w_textlistlayoutstyle_implfwd_h

#include "x/w/listlayoutmanagerfwd.H"

LIBCXXW_NAMESPACE_START

//! Passed to \ref new_listlayoutmanager "new_listlayoutmanager" for menus.

//! Based on a \ref bulleted_list "bulleted_list", with an extra trailing
//! column for the menu item's shortcut, or a submenu image.

extern const listlayoutstyle_impl &menu_list;

LIBCXXW_NAMESPACE_END

#endif
