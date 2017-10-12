/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_textlist_cellimagefwd_h
#define x_w_textlist_cellimagefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class textlist_cellimageObj;

/*! A single image cell in a \ref textlistlayoutmanager "list layout manager".


*/

typedef ref<textlist_cellimageObj> textlist_cellimage;

//! A constant \ref textlist_cellimage "image list cell".

//! \see textlist_cellimage

typedef const_ref<textlist_cellimageObj> const_textlist_cellimage;

//! A nullable pointer reference to a \ref textlist_cellimage "image list cell".

//! \see textlist_cellimage

typedef ptr<textlist_cellimageObj> textlist_cellimageptr;

//! A nullable pointer reference to a const \ref textlist_cellimage "image list cell".

//! \see textlist_cellimage

typedef const_ptr<textlist_cellimageObj> const_textlist_cellimageptr;

LIBCXXW_NAMESPACE_END

#endif
