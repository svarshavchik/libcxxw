/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menu_popup_h
#define menu_popup_h

#include "popup/popup.H"
#include "popup/popup_attachedto_handler.H"
#include "x/w/elementobj.H"
#include "x/w/listlayoutmanager.H"
#include <x/functional.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Create a popup menu and its attachment handler.

//! Shared code by the menubar layout manager, to create a new menu
//! popup and also by the menu layout manager, to create a submenu
//! popup.

template<typename creator_t, typename which_one>
static inline auto create_menu_popup(const elementimpl &e,
				     creator_t &&creator,
				     which_one &wo)
{
	return wo(e, make_function<void (const listlayoutmanager &)>
		  (std::forward<creator_t>(creator)));
}

//! Creating a top level menu.

//! Passed as a third parameter to create_menu_popup().

std::tuple<popup, ref<popup_attachedto_handlerObj> >
topmenu_popup(const elementimpl &e,
	      const function<void (const listlayoutmanager &)> &creator)
	LIBCXX_HIDDEN;

//! Creating a submenu menu.

//! Passed as a third parameter to create_menu_popup().

std::tuple<popup, ref<popup_attachedto_handlerObj> >
submenu_popup(const elementimpl &e,
	      const function<void (const listlayoutmanager &)>
			&creator)
	LIBCXX_HIDDEN;

//! Creating a popup context menu.

container
contextmenu_popup(const elementimpl &e,
		  const function<void (const listlayoutmanager &)> &creator);

LIBCXXW_NAMESPACE_END

#endif
