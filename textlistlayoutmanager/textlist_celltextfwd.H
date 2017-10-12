/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlist_celltextfwd_h
#define x_w_textlist_celltextfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class textlist_celltextObj;

/*! A single text cell in a \ref textlistlayoutmanager "list layout manager".

*/

typedef ref<textlist_celltextObj> textlist_celltext;

//! A constant \ref textlist_celltext "text list cell".

//! \see textlist_celltext

typedef const_ref<textlist_celltextObj> const_textlist_celltext;

//! A nullable pointer reference to a \ref textlist_celltext "text list cell".

//! \see textlist_celltext

typedef ptr<textlist_celltextObj> textlist_celltextptr;

//! A nullable pointer reference to a const \ref textlist_celltext "text list cell".

//! \see textlist_celltext

typedef const_ptr<textlist_celltextObj> const_textlist_celltextptr;

LIBCXXW_NAMESPACE_END

#endif
