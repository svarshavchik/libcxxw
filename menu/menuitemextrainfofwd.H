/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menuitemextrainfofwd_h
#define x_w_menuitemextrainfofwd_h

#include "x/w/namespace.H"
#include "x/w/menulayoutmanagerfwd.H"
#include <x/ptrfwd.H>
#include <utility>

LIBCXXW_NAMESPACE_START

class menuitemextrainfoObj;

/*! Additional menu item metadata

Element in column #1 of a menu. This element is either empty, shows the
menu item's shortcut, or the right-chevron indicating the presence of a
submenu.

The element is a container with the singletonlayoutmanager, containing
the appropriate visual element.

*/

typedef ref<menuitemextrainfoObj> menuitemextrainfo;

//! A constant \ref menuitemextrainfo "menu item metadata".

//! \see menuitemextrainfo

typedef const_ref<menuitemextrainfoObj> const_menuitemextrainfo;

//! A nullable pointer reference to a \ref menuitemextrainfo "menu item metadata".

//! \see menuitemextrainfo

typedef ptr<menuitemextrainfoObj> menuitemextrainfoptr;

//! A created plain menu item.

struct created_menuitem_plain;

//! The type of a created menu item.

typedef std::variant<created_menuitem_plain> created_menuitem_type_t;

LIBCXXW_NAMESPACE_END

#endif
