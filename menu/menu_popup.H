/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menu_popup_h
#define menu_popup_h

#include "popup/popup.H"
#include "x/w/elementobj.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/popup_list_appearancefwd.H"
#include <x/functional.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Create a popup menu and its attachment handler.

//! Shared code by the menubar layout manager, to create a new menu
//! popup, by the menu layout manager, to create a submenu
//! popup, and when creating a context popup menu.

template<typename creator_t, typename which_one>
static inline auto create_menu_popup(const element_impl &e,
				     creator_t &&creator,
				     const const_popup_list_appearance
				     &appearance,
				     which_one &wo)
{
	return wo(e, appearance,
		  make_function<void (const listlayoutmanager &)>
		  (std::forward<creator_t>(creator)));
}

//! Creating a top level menu.

//! Passed as a third parameter to create_menu_popup().

std::tuple<popup, ref<popupObj::handlerObj> >
topmenu_popup(const element_impl &e,
	      const const_popup_list_appearance &appearance,
	      const function<void (const listlayoutmanager &)> &creator)
	LIBCXX_HIDDEN;

//! Creating a submenu menu.

//! Passed as a third parameter to create_menu_popup().

std::tuple<popup, ref<popupObj::handlerObj> >
submenu_popup(const element_impl &e,
	      const const_popup_list_appearance &appearance,
	      const function<void (const listlayoutmanager &)>
			&creator)
	LIBCXX_HIDDEN;

//! Creating a popup context menu.

container
contextmenu_popup(const element_impl &e,
		  const const_popup_list_appearance &appearance,
		  const function<void (const listlayoutmanager &)> &creator);

LIBCXXW_NAMESPACE_END

#endif
