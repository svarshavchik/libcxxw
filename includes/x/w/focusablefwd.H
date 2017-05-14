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
//! \endcode
//!
//! The parameter to \c get_focus_after() and \c get_focus_before() is another
//! focusable display element in the same window. These functions moves
//! the tabbing order of this display lemeent.
//!
//! get_focus_after() sets this focusable element's tabbing order to be after
//! the other display element's.
//!
//! get_focus_before() sets this focuable element's tabbing order to be before
//! the other display element's. This element replaces the other element in
//! the current tabbing order, and the other display element tabbing order
//! becomes after this element's tabbing order.

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
