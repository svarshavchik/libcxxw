/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_cellfwd_h
#define x_w_list_cellfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN list_cellObj;

/*! A single cell in a \ref listlayoutmanager "list".


*/

typedef ref<list_cellObj> list_cell;

//! A constant \ref list_cell "list cell".

//! \see list_cell

typedef const_ref<list_cellObj> const_list_cell;

//! A nullable pointer reference to a \ref list_cell "list cell".

//! \see list_cell

typedef ptr<list_cellObj> list_cellptr;

//! A nullable pointer reference to a const \ref list_cell "list cell".

//! \see list_cell

typedef const_ptr<list_cellObj> const_list_cellptr;

LIBCXXW_NAMESPACE_END

#endif
