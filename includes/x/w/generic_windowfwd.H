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

//! Generic top level window methods.

//! \code
//! main_window->set_window_title("Hello world");
//!
//! main_window->set_window_title(U"Hello world");
//!
//! main_window->set_window_class("main", "myapp");
//! \endcode
//!
//! \c set_window_title() sets the top level window's title, that gets
//! specified as either a UTF-8 or a Unicode string.
//!
//! \c set_window_class() explicitly
//! sets the top level window's WM_CLASS property.
//! This is used with some window managers to set up application and window-
//! specific configurations. set_window_class()'s parameters are the
//! instance and the resource identifiers.
//!
//! The default resource identifier is the executing program's name. The
//! default instance identifier may be one of: "main", "menu", "dialog",
//! "combobox", or "tooltip".
//!
//! \code
//! main_window->on_disconnect([]{ /* ... */ });
//! \endcode
//!
//! Installs a callback that gets invoked if the connection to the X server
//! gets shut down unexpectedly.
//!
//! A generic window is also a
//! \ref container "container" and a
//! \ref drawable "drawable".
//!
//! \code
//! main_window->raise();
//!
//! main_window->lower();
//! \endcode
//!
//! Moves this window above all other windows, or below all other windows.

typedef ref<generic_windowObj, generic_windowBase> generic_window;

//! A constant generic window handler

//! \see generic_window

typedef const_ref<generic_windowObj, generic_windowBase> const_generic_window;

LIBCXXW_NAMESPACE_END

#endif
