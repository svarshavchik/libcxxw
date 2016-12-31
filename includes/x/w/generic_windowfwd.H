/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowfwd_h
#define x_w_generic_windowfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class generic_windowObj;
class generic_windowBase;

//! Generic window methods.
//!
//! \code
//!
//! // Open the default screen on the default display connection
//! auto s=window->get_screen();
//!
//! \endcode
//!
//! \c get_screen() returns the window's \ref screen "screen".

typedef ref<generic_windowObj, generic_windowBase> generic_window;

//! A constant generic window handler

//! \see generic_window

typedef const_ref<generic_windowObj, generic_windowBase> const_generic_window;

LIBCXXW_NAMESPACE_END

#endif
