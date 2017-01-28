/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_picturefwd_h
#define x_w_picturefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class pictureObj;

//! A Picture object, as specified in the X Rendering extension.

//! A picture consists of a
//! \ref drawable "drawable", a \ref pictformat "pictformat", and
//! some rendering state, and implements rendering operations.

typedef ref<pictureObj> picture;

//! A constant picture

typedef const_ref<pictureObj> const_picture;

LIBCXXW_NAMESPACE_END

#endif
