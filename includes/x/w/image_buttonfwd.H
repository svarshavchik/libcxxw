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
//!                           (bool initial, size_t value, const LIBCXX_NAMESPACE::w::busy &busy_mcguffin)
//!                           {
//!                           });
//! \endcode
//!
//! The value of the image_button is its current state. For an ordinary
//! checkbox or a radio button, 0 means that it's not selected, and 1 means
//! that it's selected. get_value() returns the current value, and
//! set_value() sets it.
//!
//! Checkboxes also have a state 2, the "indeterminate" state. The
//! indeterminate state can only be set manually with set_value(). Clicking
//! on a checkbox always changes its state between 0 and 1.
//!
//! \note
//! The state of the image button gets updated by the library's internal
//! execution thread. It's possible that calling get_value() immediately after
//! set_value() might return the old value, for a brief period of time
//! until the execution thread gets around to updating it.
//!
//! on_activate() installs a callback that gets invoked when the image button's
//! value gets changed, with \c value indicating the new state of the image
//! button. Only one callback can be installed at a time. Installing a new
//! callback replaces the previous one. Callbacks also get installed by the
//! internal execution thread. When an existing callback gets replaced it's
//! still possible that the old one might get immediately invoked, if the
//! internal execution thread was set to do that, before updating the
//! installed callback.
//!
//! A newly installed callback always gets invoked immediately with
//! the \c initial parameter set to true, and the image button's
//! current state value. All subsequent invocations pass \c false for the flag
//! and give the checkbox's or the radio button's new state value. The
//! callbacks get invoked whether the checkbox's or the radio button's state
//! gets clicked on, or directly via \c set_value().

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
