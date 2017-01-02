/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_drawablefwd_H
#define x_w_drawablefwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class drawableObj;

//! A drawable object.

//! \internal

typedef ref<drawableObj> drawable;

//! A nullable pointer reference to a \ref drawable "drawable" to the server

typedef ptr<drawableObj> drawableptr;

//! A constant \ref drawable "drawable" to the server
typedef const_ref<drawableObj> const_drawable;

//! A nullable pointer reference to a constant \ref drawable "drawable" to the server

typedef const_ptr<drawableObj> const_drawableptr;

LIBCXXW_NAMESPACE_END

#endif
