/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_shortcut_h
#define x_w_shortcut_h

#include <string>
#include <x/w/shortcutfwd.H>
#include <x/w/input_mask.H>
#include <x/w/key_eventfwd.H>

LIBCXXW_NAMESPACE_START

//! A keyboard shortcut for a \ref hotspot "hotspot".

//! Defines a keyboard shortcut for activating a hotspot no matter which
//! display element has keyboard focus.

class shortcut : public input_mask {

public:
	//! Which key activates a hotspot.

	char32_t unicode;

	//! Which key activates a hotspot

	//! A 0 key and keysym indicates no shortcut.

	uint32_t keysym;

	//! Default constructor.

	//! The default constructor does not define any actual shortcut.
	//! matches() will never return \c true.

	shortcut();

	//! Whether the shortcut is specified

	//! Return \c false for a default constructor.

	operator bool() const;

	//! Operator
	bool operator!() const { return operator bool() == false; }

	//! Defines a shortcut comprising of a single key.

	//! This is typically done only for \c \\n or \c Esc.
	//! Most shortcuts should have a modifier, like Alt.
	shortcut(char32_t unicode);

	//! Specify a shortcut comprising of a modifier and a key.

	//! The first parameter is a string that specifies a modifier.
	//! see \ref input_mask "input_mask"'s constructor.

	shortcut(const std::experimental::string_view &modifier,
		 char32_t unicode);

	//! Specify a shortcut comprising of a function key.

	//! This constructor expects the string to be "Fn", and sets keysym
	//! to the appropriate function key.
	//!
	//! The specified string may also have a "modifier-" prefix, example:
	//! "shift-F1".

	shortcut(const std::experimental::string_view &string);

	//! Internal constructor

	shortcut(size_t p,
		 const std::experimental::string_view &string);

	//! Whether a key event matches this shortcut.
	bool matches(const key_event &ke) const;
};

LIBCXXW_NAMESPACE_END

#endif