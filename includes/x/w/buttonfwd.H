/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_buttonfwd_h
#define x_w_buttonfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class buttonObj;

//! A clickable button.

//! \code
//! button->on_activate([]
//!                     (ONLY IN_THREAD,
//!                      const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &,
//!                      const INSERT_LIBX_NAMESPACE::w::busy &)
//!                     {
//!                     });
//! \endcode
//!
//! on_activate() installs a callback that gets invoked by clicking on the
//! button.
//!
//! \code
//! #include <INSERT_LIBX_NAMESPACE/w/singletonlayoutmanager.H>
//!
//! INSERT_LIBX_NAMESPACE::w::singletonlayoutmanager l=button->get_layoutmanager();
//! \endcode
//!
//! get_layoutmanager() returns the
//! \ref singletonlayoutmanager "singleton layout manager"
//! for accessing or replacing
//! the button's contents.

typedef ref<buttonObj> button;

//! A nullable pointer reference to an  button.

typedef ptr<buttonObj> buttonptr;

//! A reference to a constant  button.

//! \see button

typedef const_ref<buttonObj> const_button;

//! A nullable pointer reference to a constant  button.

//! \see button

typedef const_ptr<buttonObj> const_buttonptr;

LIBCXXW_NAMESPACE_END

#endif
