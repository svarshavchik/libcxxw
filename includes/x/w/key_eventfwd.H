/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef key_eventfwd_H
#define key_eventfwd_H

#include <x/w/busyfwd.H>
#include <x/w/connection_threadfwd.H>
#include <string_view>
#include <variant>

LIBCXXW_NAMESPACE_START

class key_event;

//! \ref all_key_events_t "all_key_events_t"  is not copyable.

struct all_key_events_is_not_copyable {

	all_key_events_is_not_copyable(const all_key_events_is_not_copyable
				       &)=delete;
};

//! Key event callback

//! There are two alternative key events. An actual key press or release,
//! that's reported as a \ref key_event "INSERT_LIBX_NAMESPACE::w::key_event";
//! and text entered via the X Input Method server, that comes in as a
//! \c std::u32string_view. The key event callback may receive one or the
//! other. Its argument is a variant of pointers to the possible events.

typedef std::variant<const key_event *,
		     const std::u32string_view *,
		     all_key_events_is_not_copyable> all_key_events_t;

//! Callbacks for reporting key events.

//! The callback returns true if it processed the key event.
//!
//! The first parameter is the key event, an actual keypress or pasted
//! text from the X input method manager.
//!
//! The second parameter indicates whether the key press or release is
//! considered to be "activation" of that key. This parameter is always
//! set to "true" for pasted X input method manager text.

typedef bool key_event_callback_t(THREAD_CALLBACK,
				  const all_key_events_t &,
				  bool,
				  const busy &mcguffin);

LIBCXXW_NAMESPACE_END
#endif
