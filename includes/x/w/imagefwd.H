/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_imagefwd_h
#define x_w_imagefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class imageObj;

//! An image

typedef ref<imageObj> image;

//! A nullable pointer reference to an image.

typedef ptr<imageObj> imageptr;

//! A reference to a constant image.

//! \see image

typedef const_ref<imageObj> const_image;

//! A nullable pointer reference to a constant image.

//! \see image

typedef const_ptr<imageObj> const_imageptr;

LIBCXXW_NAMESPACE_END

#endif
