/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_paramfwd_H
#define x_w_text_paramfwd_H

#include <x/w/namespace.H>
#include <x/w/button_eventfwd.H>
#include <x/w/key_eventfwd.H>
#include <x/w/focus.H>
#include <x/exception.H>
#include <variant>

LIBCXXW_NAMESPACE_START

class text_param;

//! The event that invoked a \ref text_hotspot "text link callback."

//! The callback gets invoked for a button event, or for a
//! focus::gained and focus::lost event, indicating that the pointer moved
//! into or out of this text section.
//!
//! The callback also gets invoked for a key event, when the link is
//! activated by a key press.

typedef std::variant<const button_event *,
		     const key_event *,
		     focus_change> text_event_t;

LIBCXXW_NAMESPACE_END

#endif
