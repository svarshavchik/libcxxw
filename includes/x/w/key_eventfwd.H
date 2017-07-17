/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef key_eventfwd_H
#define key_eventfwd_H

#include <x/w/busyfwd.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

class key_event;

//! Callbacks for reporting key events.

//! The callback returns true if it processed the key event.

typedef bool key_event_callback_t(const key_event &);

//! Callbacks for reporting pasted text from the X Input Method.

//! The callback returns true if it processed the pasted text.

typedef bool input_text_callback_t(const std::u32string_view &);

LIBCXXW_NAMESPACE_END
#endif
