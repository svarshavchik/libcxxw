/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_buttonfwd_h
#define x_w_image_buttonfwd_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/ptrfwd.H>
#include <functional>

LIBCXXW_NAMESPACE_START

class image_buttonObj;

//! Signature for an image_button activation function.

typedef std::function<void (bool, size_t, const busy &)> image_button_callback_t;

//! A simple button that display a different image when it changes state.

//! A traditional checkbox or a radio button. This object is returned
//! from a \ref factory "factory"'s create_checkbox() or create_radio()
//! method.
//!
//! \code
//! size_t v=image_button->get_value();
//!
//! image_button->set_value(1);
//!
//! image_button->on_activate([]
//!                           (bool initial, size_t value)
//!                           {
//!                           });
//! \endcode
//!
//! The value of the image_button is its current state. For an ordinary
//! checkbox or a radio button, 0 means that it's not selected, and 1 means
//! that it's selected. get_value() returns the current value, and
//! set_value() sets it.
//!
//! \note
//! The state of the image button gets updated by the library's internal
//! execution thread. Immediate after calling set_value(), calling get_value()
//! might return the old value, until the execution thread gets around to
//! updating in.
//!
//! on_activate() installs a callback that gets invoked when the image button's
//! value gets changed, with \c value indicating the new state of the image
//! button. Only one callback can be installed at a time. Installing a new
//! callback replaces the previous one. Callbacks also get installed by the
//! internal execution thread. When an existing callback gets replaced it's
//! still possibly that the old one might get immediately invoked, if the
//! internal execution thread was set to do that, before updating the
//! installed callback.
//!
//! Immediately upon the installation of a callback, it gets invoked with
//! the \c initial parameter set to true, indicating the image button's
//! current value. All subsequent invocations pass \c false for the flag.

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
