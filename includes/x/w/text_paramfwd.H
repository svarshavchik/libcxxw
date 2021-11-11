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
#include <x/w/connection_threadfwd.H>
#include <x/exception.H>
#include <x/functionalrefptrfwd.H>
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

#ifndef x_w_text_hotspot_H
#define x_w_text_hotspot_H

/*! \brief A focusable label hotspot callback

\ref focusable_label "Focusable labels" invoke these callbacks when the
corresponding parts of the focusable label gain or lose keyboard focus,
or are activated.

Hotspot handlers are callable objects that get invoked with a
\ref text_event_t "INSERT_LIBX_NAMESPACE::w::text_event_t" which indicates
the reason for invoking the callback:

- a \ref focus_change "INSERT_LIBX_NAMESPACE::w::focus_change" value, either
\c INSERT_LIBX_NAMESPACE::w::focus_changed::gained or
\c INSERT_LIBX_NAMESPACE::w::focus_changed::gained indicating that the
hotspot area gains or loses keyboard or pointer focus.

- a pointer to either a
\ref key_event "INSERT_LIBX_NAMESPACE::w::key_event" or a
\ref key_event "INSERT_LIBX_NAMESPACE::w::button_event" that activates or
clicks on the hotspot.

The hotspot handler must return another
\ref text_param "INSERT_LIBX_NAMESPACE::w::text_param" (but without any
hotspot markers of its own). Empty text results in no change to the visual
appearance. A non-empty text replaces the hotspot's text with the returned
value.

 */
typedef functionref<text_param(THREAD_CALLBACK,
			       const text_event_t &)> text_hotspot;

/*! \brief A nullable pointer to a focusable label hotspot callback

\see text_hotspot
*/

typedef functionptr<text_param(THREAD_CALLBACK,
			       const text_event_t &)> text_hotspotptr;
#endif

/*!
** All hotspot callbacks
**
** create_focusable_label() takes a container of hotspots to bind them to
** the placeholders in the text parameter.
*/

typedef std::unordered_map<label_hotspot_id, text_hotspot> label_hotspots_t;


LIBCXXW_NAMESPACE_END

#endif
