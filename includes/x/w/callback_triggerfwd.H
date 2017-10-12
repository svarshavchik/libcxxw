/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_callback_triggerfwd_h
#define x_w_callback_triggerfwd_h

#include <x/w/namespace.H>
#include <x/w/button_eventfwd.H>
#include <x/w/key_eventfwd.H>

#include <functional>
#include <variant>

LIBCXXW_NAMESPACE_START

//! Placeholder for an \ref callback_trigger_initial "initial callback".

struct initial {};

//! What triggered a callback activation.

//! This parameter specifies the reason for a callback's activation: a
//! key event, a button event,  or something else. The default (monostate)
//! value indicates a default reason that does not have any associated
//! metadata, for example a list item
//! getting deselected because it's getting removed from the list.
//!
//! \see callback_trigger_key_event
//!
//! \see callback_trigger_button_event
//!
//! \see callback_trigger_initial

typedef std::variant<std::monostate,
		     const key_event *,
		     const button_event *,
		     initial
		     > callback_trigger_t;

//! \ref callback_trigger_t "callback_trigger_t" has a \ref key_event "const INSERT_LIBX_NAMESPACE::w::key_event &".

//! std::variant index where one might find a \ref key_event "key_event".

constexpr size_t callback_trigger_key_event=1;

//! \ref callback_trigger_t "callback_trigger_t" has a \ref button_event "const INSERT_LIBX_NAMESPACE::w::button_event &".

//! std::variant index where one might find a \ref button_event "button_event".

constexpr size_t callback_trigger_button_event=2;

//! \ref callback_trigger_t "callback_trigger_t" initial invocation.

//! std::variant index used only with \ref image_button "image button"s, and
//! indicates the callback's initial invocation.

constexpr size_t callback_trigger_initial=3;

LIBCXXW_NAMESPACE_END

#endif
