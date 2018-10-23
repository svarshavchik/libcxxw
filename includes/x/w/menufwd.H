/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menufwd_h
#define x_w_menufwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC menuObj;

/*! A menu button in a \ref menubarlayoutmanager "menu bar".

\code
INSERT_LIBX_NAMESPACE::w::listlayoutmanager l=m->get_layoutmanager();
\endcode

This display element represents a menu in a menu bar, a container
with \ref listlayoutmanager "list layout manager" that manages the items
in the menu.

\code
INSERT_LIBX_NAMESPACE::w::listlayoutmanager lm=m->get_layoutmanager();
\endcode

get_layoutmanager() returns the
\ref listlayoutmanager "list layout manager" for the popup menu.

\code
m->on_popup_state_update([]
                         (ONLY IN_THREAD,
                          const INSERT_LIBX_NAMESPACE::w::element_state &s,
                          const INSERT_LIBX_NAMESPACE::w::busy &mcguffin)
                         {
                         });
\endcode

This installs an on_state_update() callback for the menu's popup window.
See \ref element "on_state_update()" for more information. This provides the
means of having a callback invoked whenever the popup menu is shown or hidden.

*/

typedef ref<menuObj> menu;

//! A constant \ref menu "menu button".

//! \see menu

typedef const_ref<menuObj> const_menu;

//! A nullable pointer reference to a \ref menu "menu button".

//! \see menu

typedef ptr<menuObj> menuptr;

//! A nullable pointer reference to a const \ref menu "menu button".

//! \see menu

typedef const_ptr<menuObj> const_menuptr;

LIBCXXW_NAMESPACE_END

#endif
