/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_buttonfwd_h
#define x_w_image_buttonfwd_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/alignment.H>
#include <x/w/border_arg.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>
#include <string>
#include <vector>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC image_buttonObj;

//! Signature for an image_button activation function.

typedef functionref<void (THREAD_CALLBACK, size_t,
			  const callback_trigger_t &,
			  const busy &)> image_button_callback_t;

//! A simple button that display a different image when it changes state.

//! A traditional checkbox or a radio button. This object is returned
//! from a \ref factory "factory"'s create_checkbox() or create_radio()
//! method.
//!
//! create_checkbox() and create_radio() take the same parameters, except
//! that create_radio() takes an additional, initial parameter:
//! a \ref radio_group "INSERT_LIBX_NAMESPACE::w::radio_group" object, an
//! opaque object that links all radio buttons into a single group. The
//! common parameters are:
//!
//! - an optional \ref factory "factory object" for creating the button's
//! label, the description of the button that appears next to it.
//!
//! - a \c std::vector<std::string> that specifies custom images
//! for a checkbox or a radio button that uses custom graphics.
//! See \ref image "INSERT_LIBX_NAMESPACE::w::image" for more information.
//! Each
//! string names a file with an image for the corresponding state. The default
//! value for this parameter creates three images for checkboxes: set, unset
//! and intermediate. Radio buttons have two default image/states:
//! selected and unselected.
//!
//! - a \ref valign "vertical alignment setting".
//!
//! Each parameter is individually optional (the initial
//! \ref radio_group "INSERT_LIBX_NAMESPACE::w::radio_group" parameter
//! to create_radio() is mandatory, and appears before these three parameters)
//! but these parameters must appear in the given order (this is done with
//! some aggressive overloading).
//!
//! A sole factory parameter is the usual case. The factory is expected to
//! create a single widget, typically a \ref label "label", that
//! appears next to the button's image. Without a factory-created label,
//! the button image appears by itself, unadorned.
//!
//! Internally, the \c INSERT_LIBX_NAMESPACE::w::image_button is a one-row
//! \ref gridlayoutmanager "grid", with the actual image button display
//! element, and an optional label.
//!
//! \code
//! size_t v=image_button->get_value();
//!
//! image_button->set_value(1);
//!
//! image_button->set_value(IN_THREAD, 1);
//!
//! image_button->set_value(IN_THREAD, 1, trigger);
//!
//! image_button->on_activate([]
//!                           (ONLY IN_THREAD,
//!                            size_t value,
//!                            const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger,
//!                            const INSERT_LIBX_NAMESPACE::w::busy &busy_mcguffin)
//!                           {
//!                                  if (trigger.index() == INSERT_LIBX_NAMESPACE::w::callback_trigger_initial)
//!                                  {
//!                                  }
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
//! the \c trigger parameter specifying the \c INSERT_LIBX_NAMESPACE::w::callback_trigger_initial
//! as the reason for the callback.
//! All subsequent invocations pass a different reason,
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
