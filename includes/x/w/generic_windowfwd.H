/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowfwd_h
#define x_w_generic_windowfwd_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC generic_windowObj;
class LIBCXX_PUBLIC generic_windowBase;

//! on_stabilized() callback type.

typedef functionref<void (THREAD_CALLBACK,
			  const busy &)> stabilized_callback_t;

//! Generic top level window methods.

//! \ref main_window "INSERT_LIBX_NAMESPACE::w::main_window" is the object
//! that represents a top-level window. Some of its functionality is implemented
//! in this superclass.
//!
//! \code
//! main_window->set_window_title("Hello world");
//!
//! main_window->set_window_title(U"Hello world");
//!
//! main_window->set_window_class("main", "myapp");
//!
//! main_window->set_window_type("toolbar");
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
//! \c set_window_type() overrides the _NET_WM_WINDOW_TYPE property.
//! Its argument is a comma/whitespace separated list of known window types:
//!
//! - "normal"
//!
//! - "toolbar"
//!
//! - "menu"
//!
//! - "utility"
//!
//! - "splash"
//!
//! - "dialog"
//!
//! - "notification"
//!
//! \code
//! main_window->on_disconnect([]{ /* ... */ });
//! \endcode
//!
//! Installs a callback that gets invoked if the connection to the X server
//! gets shut down unexpectedly.
//!
//! \code
//! main_window->on_stabilized([]
//!                            (ONLY IN_THREAD,
//!                             const LIBCXX_NAMESPACE::w::busy &mcguffin)
//!                             {
//!                             });
//! \endcode
//!
//! on_stabilized() arranges for its callback to get invoked after the
//! window becomes visible and its screen position and size is computed
//! and fixed.
//!
//! The callback gets invoked with no delay if this already occured.
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
//!
//! \par Selections
//!
//! \code
//! bool flag=main_window->selection_has_owner("SECONDARY");
//! \endcode
//!
//! For convenience, this is forwarded to this screen's
//! \ref connection "connection".
//!
//! \code
//! bool flag=main_window->selection_can_be_received();
//! \endcode
//!
//! Returns true if the current keyboard focus is in a display
//! element that's capable of receiving a converted X selection.
//! Currently, only \ref input_field "input fields" accept converted
//! X selections as pasted text.
//!
//! \note
//! Unless selection_can_be_received() gets invoked from the connection thread,
//! the keyboard focus can change at any time and this because out of date.
//!
//! \code
//! main_window->receive_selection();
//!
//! main_window->receive_selection(IN_THREAD)
//! \endcode
//!
//! Presumably we've checked if selection_can_be_received(), so this
//! starts the process of converting the X selection into the display element.
//!
//! \note
//! receive_selection() returns immediately after sending a request to the
//! selection's current owner. The display element gets updated later, when
//! the owner's response, with the converted selection's data gets received.
//!
//! \code
//! bool flag=main_window->cut_or_copy_selection(cut_or_copy_op::available);
//!
//! bool flag=main_window->cut_or_copy_selection(IN_THREAD, INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::available);
//!
//! main_window->cut_or_copy_selection(INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::copy);
//!
//! main_window->cut_or_copy_selection(IN_THREAD, INSERT_LIBX_NAMESPACE::w::cut_or_copy_op::copy);
//! \endcode
//!
//! cut_or_copy_selection() executes a cut or a copy operation specified
//! by the parameter.
//!
//! - cut_or_copy_op::available returns \c true if the current input focus
//! is in a display element that has something to copy or cut, and
//! \c false otherwise.
//!
//! - cut_or_copy_op::copy and cut_or_copy_op::cut start the copy or the cut
//! process, and always returns true.
//!
//! \note
//! cut_or_copy_op::available is meaningful only IN_THREAD, since things can
//! change at any time, and a copy or a cut actually takes place only if it
//! is available.

typedef ref<generic_windowObj, generic_windowBase> generic_window;

//! A constant generic window handler

//! \see generic_window

typedef const_ref<generic_windowObj, generic_windowBase> const_generic_window;

LIBCXXW_NAMESPACE_END

#endif
