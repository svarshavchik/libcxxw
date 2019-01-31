/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_cellimagefwd_h
#define x_w_list_cellimagefwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN list_cellimageObj;

/*! A single image cell in a \ref listlayoutmanager "list layout manager".


*/

typedef ref<list_cellimageObj> list_cellimage;

//! A constant \ref list_cellimage "image list cell".

//! \see list_cellimage

typedef const_ref<list_cellimageObj> const_list_cellimage;

//! A nullable pointer reference to a \ref list_cellimage "image list cell".

//! \see list_cellimage

typedef ptr<list_cellimageObj> list_cellimageptr;

//! A nullable pointer reference to a const \ref list_cellimage "image list cell".

//! \see list_cellimage

typedef const_ptr<list_cellimageObj> const_list_cellimageptr;

LIBCXXW_NAMESPACE_END

#endif
