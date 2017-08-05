/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef key_eventfwd_H
#define key_eventfwd_H

#include <x/w/busyfwd.H>
#include <string_view>
#include <variant>

LIBCXXW_NAMESPACE_START

class key_event;

//! Key event callback

//! There are two alternative key events. An actual key press or release,
//! that's reported as a \ref key_event "INSERT_LIBX_NAMESPACE::w::key_event";
//! and text entered via the X Input Method server, that comes in as a
//! \c std::u32string_view. The key event callback may receive one or the
//! other. Its argument is a variant of pointers to the possible events.

typedef std::variant<const key_event *,
		     const std::u32string_view *> all_key_events_t;

//! Callbacks for reporting key events.

//! The callback returns true if it processed the key event.

typedef bool key_event_callback_t(const all_key_events_t &,
				  const busy &mcguffin);

LIBCXXW_NAMESPACE_END
#endif
