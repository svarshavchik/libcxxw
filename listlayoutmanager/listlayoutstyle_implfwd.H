/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlistlayoutstyle_implfwd_h
#define x_w_textlistlayoutstyle_implfwd_h

#include "x/w/listlayoutmanagerfwd.H"

LIBCXXW_NAMESPACE_START

//! Passed to \ref new_listlayoutmanager "new_listlayoutmanager" for menus.

//! Based on a \ref bulleted_list "bulleted_list", with an extra trailing
//! column for the menu item's shortcut, or a submenu image.

extern const listlayoutstyle_impl &menu_list LIBCXX_HIDDEN;

//! Passed to \ref new_listlayoutmanager "new_listlayoutmanager" for combo-boxes.

//! Based on a \ref highlighted_list "highlighted_list", but doesn't allow
//! hierindent specifications.
extern const listlayoutstyle_impl &combobox_list LIBCXX_HIDDEN;

struct LIBCXX_HIDDEN textlist_rowinfo;

LIBCXXW_NAMESPACE_END

#endif
