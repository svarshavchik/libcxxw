/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menulayoutmanagerfwd_h
#define x_w_menulayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class menulayoutmanagerObj;

/*! Layout manager for a menu popup.

Inherits from the list layout manager. Do not use the list layout manager's
methods, only use this class's methods to manipulate the menu.

*/

typedef ref<menulayoutmanagerObj> menulayoutmanager;

//! A constant \ref menulayoutmanager "menu layout manager".

//! \see menulayoutmanager

typedef const_ref<menulayoutmanagerObj> const_menulayoutmanager;

//! A nullable pointer reference to a \ref menulayoutmanager "menu layout manager".

//! \see menulayoutmanager

typedef ptr<menulayoutmanagerObj> menulayoutmanagerptr;

//! A nullable pointer reference to a const \ref menulayoutmanager "menu layout manager".

//! \see menulayoutmanager

typedef const_ptr<menulayoutmanagerObj> const_menulayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
