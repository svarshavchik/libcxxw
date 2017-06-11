/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerfwd_h
#define x_w_listlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class listlayoutmanagerObj;

/*! The list layout manager.

*/

typedef ref<listlayoutmanagerObj> listlayoutmanager;

//! A constant list layout manager.

//! \see listlayoutmanager

typedef const_ref<listlayoutmanagerObj> const_listlayoutmanager;

class listlayoutstyle;

class new_listlayoutmanager;

//! The list highlights items to indicate they're selected

extern const listlayoutstyle &highlighted_list;

LIBCXXW_NAMESPACE_END

#endif
