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
#include <x/w/text_hotspotfwd.H>
#include <x/exception.H>
#include <variant>
#include <string_view>

LIBCXXW_NAMESPACE_START

class text_param;

struct start_hotspot;
struct end_hotspot;

/*!
** Identifiers for \ref focusable_label "focusable label" hotspots
**
** Each hotspot is identified by a unique identifier when constructing
** a \ref text_param "text parameter" for a focusable. This can be an
** integer or a literal string.
**
** \note
** The literal string must exist as long as the text_param remains in scope.
*/

typedef std::variant<int, std::string_view> label_hotspot_id;

/*!
** All hotspot callbacks
**
** create_focusable_label() takes a container of hotspots to bind them to
** the placeholders in the text parameter.
*/

typedef std::unordered_map<label_hotspot_id, text_hotspot> label_hotspots_t;

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
