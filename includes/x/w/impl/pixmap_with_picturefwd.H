/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_pixmap_with_picturefwd_h
#define x_w_impl_pixmap_with_picturefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN pixmap_with_pictureObj;

/*! A pixmap with a cached picture.

A subclass of a \ref pixmap "pixmap" that's always a const reference.
As such, \c pixmap_with_picture is a \c const_ref.

Overrides pixmapObj's superclass's create_picture(), to return a cached
const picture. Since this is always a \c const ovject, nothing needs to be
done to override the non_const create_picture() method.

*/

typedef const_ref<pixmap_with_pictureObj> pixmap_with_picture;

LIBCXXW_NAMESPACE_END

#endif
