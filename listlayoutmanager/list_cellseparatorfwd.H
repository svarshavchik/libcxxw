/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_cellseparatorfwd_h
#define x_w_list_cellseparatorfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class list_cellseparatorObj;

/*! A separator cell in a \ref listlayoutmanager "list layout manager".

*/

typedef ref<list_cellseparatorObj> list_cellseparator;

//! A constant \ref list_cellseparator "separator list cell".

//! \see list_cellseparator

typedef const_ref<list_cellseparatorObj> const_list_cellseparator;

//! A nullable pointer reference to a \ref list_cellseparator "separator list cell".

//! \see list_cellseparator

typedef ptr<list_cellseparatorObj> list_cellseparatorptr;

//! A nullable pointer reference to a const \ref list_cellseparator "separator list cell".

//! \see list_cellseparator

typedef const_ptr<list_cellseparatorObj> const_list_cellseparatorptr;

LIBCXXW_NAMESPACE_END

#endif
