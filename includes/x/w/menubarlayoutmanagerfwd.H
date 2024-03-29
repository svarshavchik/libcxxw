/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarlayoutmanagerfwd_h
#define x_w_menubarlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC menubar_lock;
class LIBCXX_PUBLIC menubarlayoutmanagerObj;

/*! Menu bar layout manager.

A layout manager for a \ref main_window "main window"'s menu bar.

\code
INSERT_LIBX_NAMESPACE::w::menubarlayoutmanager mb=main_window->get_menubarlayoutmanager();

INSERT_LIBX_NAMESPACE::w::menubarfactory f=mb->append_menus();

INSERT_LIBX_NAMESPACE::w::menubarfactory f=mb->insert_menus(0);

INSERT_LIBX_NAMESPACE::w::menubarfactory f=mb->append_right_menus();

INSERT_LIBX_NAMESPACE::w::menubarfactory f=mb->insert_right_menus(0);

\endcode

A main window's menu bar has two sections, aligned against the left and the
right margin.

append_menus() returns a \ref menubarfactory "factory" that adds new menus
to the end of the left-aligned section.

insert_menus() returns a factory that adds new
menus before an existing menu in the left-aligned section.

append_right_menus() returns a \ref menubarfactory "factory" that adds new menus
to the end of the right-aligned section.

insert_right_menus() returns a factory that adds new
menus before an existing menu in the right-aligned section.

\code
mb->remove_menu(0);

mb->remove_right_menu(0);

\endcode

remove_menu() and remove_right_menu() remove an existing menu from the
corresponding section of the menu bar.

\code
size_t n=mb->get_menus();
size_t n=mb->get_right_menus();

INSERT_LIBX_NAMESPACE::w::menu m=mb->get_menu(0);
INSERT_LIBX_NAMESPACE::w::menu m=mb->get_right_menu(0);
\endcode

get_menus() and get_right_menus() return the number of menus on the left
and the right side of the menu bar. get_menu() and get_right_menu()
return the approrate \ref menu "menu".

*/

typedef ref<menubarlayoutmanagerObj> menubarlayoutmanager;

//! A constant \ref menubarlayoutmanager "menu bar layout manager".

//! \see menubarlayoutmanager

typedef const_ref<menubarlayoutmanagerObj> const_menubarlayoutmanager;

//! A nullable pointer reference to a \ref menubarlayoutmanager "menu bar layout manager".

//! \see menubarlayoutmanager

typedef ptr<menubarlayoutmanagerObj> menubarlayoutmanagerptr;

//! A nullable pointer reference to a const \ref menubarlayoutmanager "menu bar layout manager".

//! \see menubarlayoutmanager

typedef const_ptr<menubarlayoutmanagerObj> const_menubarlayoutmanagerptr;

class LIBCXX_PUBLIC menu_bar_lock;

LIBCXXW_NAMESPACE_END

#endif
