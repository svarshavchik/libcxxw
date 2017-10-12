/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlist_cellfwd_h
#define x_w_textlist_cellfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class textlist_cellObj;

/*! A single cell in a \ref listlayoutmanager "list".


*/

typedef ref<textlist_cellObj> textlist_cell;

//! A constant \ref textlist_cell "list cell".

//! \see textlist_cell

typedef const_ref<textlist_cellObj> const_textlist_cell;

//! A nullable pointer reference to a \ref textlist_cell "list cell".

//! \see textlist_cell

typedef ptr<textlist_cellObj> textlist_cellptr;

//! A nullable pointer reference to a const \ref textlist_cell "list cell".

//! \see textlist_cell

typedef const_ptr<textlist_cellObj> const_textlist_cellptr;

LIBCXXW_NAMESPACE_END

#endif
