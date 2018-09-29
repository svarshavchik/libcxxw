/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_celltextfwd_h
#define x_w_list_celltextfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN list_celltextObj;

/*! A single text cell in a \ref listlayoutmanager "list layout manager".

*/

typedef ref<list_celltextObj> list_celltext;

//! A constant \ref list_celltext "text list cell".

//! \see list_celltext

typedef const_ref<list_celltextObj> const_list_celltext;

//! A nullable pointer reference to a \ref list_celltext "text list cell".

//! \see list_celltext

typedef ptr<list_celltextObj> list_celltextptr;

//! A nullable pointer reference to a const \ref list_celltext "text list cell".

//! \see list_celltext

typedef const_ptr<list_celltextObj> const_list_celltextptr;

LIBCXXW_NAMESPACE_END

#endif
