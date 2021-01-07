/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_internalfwd_h
#define image_button_internalfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN image_button_internalObj;

//! A simple button that display a different image when it changes state.

//! This is an internal implementation element. The public image_button
//! is really a focusframe hotspot, that contains this display element.
//! The public image_button is also the owner of this focusable object.
//!
//! Also, image_button_internal's implementation object inherits from the
//! hotspot implementation object, however it's the public image_button
//! that inherits from the public hotspot object.

typedef ref<image_button_internalObj> image_button_internal;

//! A nullable pointer reference to an image button.

typedef ptr<image_button_internalObj> image_button_internalptr;

//! A reference to a constant image button.

//! \see image_button_internal

typedef const_ref<image_button_internalObj> const_image_button_internal;

//! A nullable pointer reference to a constant image button.

//! \see image_button_internal

typedef const_ptr<image_button_internalObj> const_image_button_internalptr;

LIBCXXW_NAMESPACE_END

#endif
