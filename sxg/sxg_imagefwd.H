/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef sxg_imagefwd_H
#define sxg_imagefwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class sxg_imageObj;

/*!
A rendered SXG image.

This is returned by \ref sxg_parser "SXG parser"'s render() method.

*/

typedef ref<sxg_imageObj> sxg_image;

//! A nullable pointer reference to a \ref sxg_image "sxg image".

typedef ptr<sxg_imageObj> sxg_imageptr;

//! A reference to a const \ref sxg "sxg image".

typedef const_ref<sxg_imageObj> const_sxg_image;

//! A nullable pointer reference to a const \ref sxg "sxg image".

typedef const_ptr<sxg_imageObj> const_sxg_imageptr;

LIBCXXW_NAMESPACE_END

#endif
