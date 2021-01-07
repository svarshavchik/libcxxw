/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itembuttonfwd_h
#define x_w_itembuttonfwd_h

#include "x/w/namespace.H"
#include "x/w/itemlayoutmanagerfwd.H"
#include <x/ptrfwd.H>
#include <vector>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN itembuttonObj;

/*! A single button in the item layout manager

Internal display element a container with a button, and a label for an item.

*/

typedef ref<itembuttonObj> itembutton;

//! A constant \ref itembutton "item button".

//! \see itembutton

typedef const_ref<itembuttonObj> const_itembutton;

//! A nullable pointer reference to a \ref itembutton "item button".

//! \see itembutton

typedef ptr<itembuttonObj> itembuttonptr;

//! A nullable pointer reference to a const \ref itembutton "item button".

//! \see itembutton

typedef const_ptr<itembuttonObj> const_itembuttonptr;

LIBCXXW_NAMESPACE_END

#endif
