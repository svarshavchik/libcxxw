/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagetabgridlayoutmanagerfwd_h
#define x_w_pagetabgridlayoutmanagerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN pagetabgridlayoutmanagerObj;

/*! The grid layout manager for a \ref booklayoutmanager "book layout manager"'s page tab.


*/

typedef ref<pagetabgridlayoutmanagerObj> pagetabgridlayoutmanager;

//! A constant \ref pagetabgridlayoutmanager "book page tab grid layoutmanager".

//! \see pagetabgridlayoutmanager

typedef const_ref<pagetabgridlayoutmanagerObj> const_pagetabgridlayoutmanager;

//! A nullable pointer reference to a \ref pagetabgridlayoutmanager "book page tab grid layoutmanager".

//! \see pagetabgridlayoutmanager

typedef ptr<pagetabgridlayoutmanagerObj> pagetabgridlayoutmanagerptr;

//! A nullable pointer reference to a const \ref pagetabgridlayoutmanager "book page tab grid layoutmanager".

//! \see pagetabgridlayoutmanager

typedef const_ptr<pagetabgridlayoutmanagerObj> const_pagetabgridlayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
