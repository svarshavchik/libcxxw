/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_popup_list_appearance_h
#define x_w_popup_list_appearance_h

#include <x/w/popup_list_appearancefwd.H>
#include <x/w/popup_list_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref popup_list_appearance "popup list appearance object".

//! Refer to this class as combobox_appearance::base

class popup_list_appearance_base : public ptrref_base {

public:

	//! Default theme for a popup menu.

	static const_popup_list_appearance menu_theme();

	//! Default theme for a submenu.

	static const_popup_list_appearance submenu_theme();

	//! Default theme for a context menu.

	static const_popup_list_appearance contextmenu_theme();
};

LIBCXXW_NAMESPACE_END

#endif
