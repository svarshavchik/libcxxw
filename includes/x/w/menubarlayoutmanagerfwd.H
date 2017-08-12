/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarlayoutmanagerfwd_h
#define x_w_menubarlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class menubarlayoutmanagerObj;

/*! Menu bar layout manager.

A layout manager for a \ref main_window "main window"'s menu bar.

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

class menu_bar_lock;

LIBCXXW_NAMESPACE_END

#endif
