/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagetabfwd_h
#define x_w_pagetabfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class pagetabObj;

/*! An element in a \ref pagetabgridlayoutmanager "pagetabgridlayoutmanager".

Represents a tab for a page in a \ref booklayoutmanager "booklayoutmanager".
Uses pagetabsingletonlayoutmanager_implObj as its layout manager.

*/

typedef ref<pagetabObj> pagetab;

//! A constant \ref pagetab "booklayoutmanager page tab element".

//! \see pagetab

typedef const_ref<pagetabObj> const_pagetab;

//! A nullable pointer reference to a \ref pagetab "booklayoutmanager page tab element".

//! \see pagetab

typedef ptr<pagetabObj> pagetabptr;

//! A nullable pointer reference to a const \ref pagetab "booklayoutmanager page tab element".

//! \see pagetab

typedef const_ptr<pagetabObj> const_pagetabptr;

LIBCXXW_NAMESPACE_END

#endif
