/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementfwd_h
#define x_w_elementfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class elementObj;

//! Methods common to all display elements.

//! \code
//! e->appdata=INSERT_LIBX_NAMESPACE::mcguffinstash<std::string>::create();
//!
//! INSERT_LIBX_NAMESPACE::w::screen screen=e->get_screen();
//!
//! e->show();
//!
//! e->show_all();
//!
//! e->hide();
//!
//! e->hide_all();
//!
//! \endcode
//!
//! \c appdata is an opaque object pointer for the application's use.
//! The library does not use it in any way.
//!
//! get_screen() returns the element's \ref screen "screen object".
//!
//! show() and hide() displays or hides the display element. show_all() and
//! hide_all() also shows or hides any other display elements contained
//! in this one (this display element is a \ref container "container").
//!
//! \code
//! auto p=e->set_background_color(INSERT_LIBX_NAMESPACE::w::rgb(INSERT_LIBX_NAMESPACE::w::rgb::maximum(), 0, 0));
//!
//! e->set_background_color("action_color1");
//!
//! e->remove_background_color();
//! \endcode
//!
//! set_background_color() sets this element's background to an
//! \ref rgb "INSERT_LIBX_NAMESPACE::rgb"-specified color, or to a theme
//! color of the given name. remove_background_color() removes the element's
//! existing background color, reverting it to the default one.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=element->get_busy_mcguffin();
//!
//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=element->connection_mcguffin();
//! \endcode
//!
//! get_busy_mcguffin() a \ref busy "mcguffin that blocks processing" of all
//! keyboard
//! or pointer button press events, until the last reference to the mcguffin
//! goes out of scope and it gets destroyed.
//!
//! connection_mcguffin() returns a mcguffin for the underlying connection
//! to the display server. Installing a destructor callback on this mcguffin
//! results in the callback getting invoked when the connection to the
//! underlying display server is closed and the internal execution thread,
//! for the connection, stops.
//!
//! \par Pointer focus callback
//!
//! \code
//! element->on_pointer_focus([]
//!                           (INSERT_LIBX_NAMESPACE::w::focus_change)
//!                           {
//!                               // ...
//!                           });
//! \endcode
//!
//! Installs a callback that reports the element's pointer focus state
//! (when the pointer moves in and out of the display element).
//!
//! The callback also gets invoked immediately to report the element's
//! current focus state.
//!
//! \note
//! Well, not immediately. The callbacks get invoked and updated by an
//! internal execution thread, and the callback's initial invocation will
//! likely occur after on_pointer_focus() returns.
//!
//! Only one callback may get installed at a time. Installing a callback
//! replaces the previous callback. Since the callbacks are handled by
//! the aforementioned internal execution thread, it is possible that
//! the previous callback still gets invoked after on_pointer_focus()
//! returns, if the execution thread was about to call it. In any case,
//! after the execution thread invokes the replacement callback once, the
//! previous one is a distant memory.
//!
//! \note
//! The callback cannot capture references to its display element, any of its
//! parent display elements, or any of its child display elements (if the
//! element is a container). This creates a circular references. Weak captures
//! must be used.

typedef ref<elementObj> element;

//! A nullable pointer reference to an element.

typedef ptr<elementObj> elementptr;

//! A reference to a constant element.

//! \see element

typedef const_ref<elementObj> const_element;

//! A nullable pointer reference to a constant element.

//! \see element

typedef const_ptr<elementObj> const_elementptr;

LIBCXXW_NAMESPACE_END

#endif
