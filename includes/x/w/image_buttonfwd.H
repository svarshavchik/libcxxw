/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_buttonfwd_h
#define x_w_image_buttonfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class image_buttonObj;

//! A simple button that display a different image when it changes state.

//! A traditional checkbox or a radio button. This object is returned
//! from a \ref factory "factory"'s create_checkbox() or create_radio()
//! method.

typedef ref<image_buttonObj> image_button;

//! A nullable pointer reference to an image button.

typedef ptr<image_buttonObj> image_buttonptr;

//! A reference to a constant image button.

//! \see image_button

typedef const_ref<image_buttonObj> const_image_button;

//! A nullable pointer reference to a constant image button.

//! \see image_button

typedef const_ptr<image_buttonObj> const_image_buttonptr;

LIBCXXW_NAMESPACE_END

#endif
