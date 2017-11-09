/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusablefwd_h
#define x_w_focusablefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class focusableObj;
class focusableImplObj;

//! Methods common to all display element that can receive input focus.

//! \code
//! f->set_enabled(false);
//! \endcode
//!
//! \c set_enabled(false) blocks this display element from receiving input
//! focus. The input focus gets moved to another display element if this
//! element has input focus at the time it is disabled.
//!
//! \c set_enabled(true) makes this focusable element enabled for processing
//! keyboard and button press events.
//!
//! Disabled focusable elements are drawn at half intensity. \c set_enabled()
//! redraws the display elements accordingly.
//!
//! \code
//! f->get_focus_after(other);
//!
//! f->get_focus_before(other);
//!
//! f->get_focus_first();
//!
//! f->request_focus();
//!
//! \endcode
//!
//! The parameter to \c get_focus_after() and \c get_focus_before() is another
//! focusable display element in the same window. These functions move
//! the tabbing order of this display lemeent.
//!
//! get_focus_after() sets this focusable element's tabbing order to be after
//! the other display element's. When the other display element has input
//! focus, TAB moves the focus to this focusable element, then whichever other
//! element originally followed the other element in the tabbing order.
//!
//! get_focus_before() sets this focusable element's tabbing order to be before
//! the other display element's. This element replaces the other element in
//! the current tabbing order, and the other display element tabbing order
//! becomes after this element's tabbing order.
//!
//! get_focus_first() set this focusable element's tabbing order to be the
//! first tabbable display element in its window.
//!
//! request_focus() moves the current keyboard focus to this focusable
//! element, if it's enabled and visible.
//!
//! \code
//! #include <INSERT_LIBX_NAMESPACE/w/focus.H>
//!
//! f->on_keyboard_focus([]
//!                      (const INSERT_LIBX_NAMESPACE::w::focus_change &f)
//!                      {
//!                         if (INSERT_LIBX_NAMESPACE::w::in_focus(f))
//!                              // ...
//!                         // ...
//!                      });
//!
//! #include <INSERT_LIBX_NAMESPACE/w/key_event.H>
//! f->on_key_event([]
//!                      (const INSERT_LIBX_NAMESPACE::w::all_key_events_t &e,
//!                       const busy &mcguffin)
//!                      {
//!                         // ...
//!
//!                         return true;
//!                      });
//! \endcode
//!
//! on_keyboard_focus() installs a callback that reports the display element's
//! focus change events. Immediately upon installation, the callback gets
//! invoked to report the element's most recent focus change event, and gets
//! invoke to report all future ones.
//!
//! Only one focus callback can be installed. Installing another keyboard
//! focus change callback replaces any existing one.
//!
//! on_key_event() installs a callback that receives key events. There are
//! two possible key events: a
//! \ref key_event "key press and release event", or
//! text entered via an X Input Method server, that comes in as a
//! \c std::u32string_view. The parameter to a callback is a
//! \c std::variant of the two, defined as a
//! \ref all_key_events_t "INSERT_LIBX_NAMESPACE::w::all_key_events_t".
//! This callback gets invoked only when the display element
//! has input focus. The
//! on_key_event() callback should return \c true if the key event
//! has been processed/consumed by the callback, and will not
//! result in any further processing. Returning \c false results in the
//! default processing for the respective key/text input.
//!
//! \code
//! f->autofocus(true);
//! \endcode
//!
//! autofocus() sets whether the display element is eligible for automatically
//! getting the keyboard focus when its main window gets shown. The first
//! display element with autofocus gets the keyboard focus.
//!
//! The default autofocus() setting depends on the display element.

typedef ref<focusableObj> focusable;

//! A nullable pointer reference to a focusable display element.

typedef ptr<focusableObj> focusableptr;

//! A reference to a constant focusable display element.

//! \see focusable

typedef const_ref<focusableObj> const_focusable;

//! A nullable pointer reference to a constant focusable display element.

//! \see focusable

typedef const_ptr<focusableObj> const_focusableptr;

LIBCXXW_NAMESPACE_END

#endif
