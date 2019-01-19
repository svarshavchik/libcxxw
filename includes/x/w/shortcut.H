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

	explicit operator bool() const;

	//! Operator
	bool operator!() const { return operator bool() == false; }

	//! Defines a shortcut comprising of a single key.

	//! This is typically done only for \c \\n or \c Esc.
	//! Most shortcuts should have a modifier, like Alt.
	shortcut(char32_t unicode);

	//! Specify a shortcut comprising of a modifier and a key.

	//! The first parameter is a string that specifies a modifier.
	//! see \ref input_mask "input_mask"'s constructor.

	shortcut(const std::string_view &modifier,
		 char32_t unicode);

	//! Specify a shortcut as a single string.

	//! The constructor can be either a single (UTF-8) character, or
	//! "Fn", and sets the unicode shortcut to the character, or the keysym
	//! to the appropriate function key.
	//!
	//! The specified string may also have a "modifier-" prefix, example:
	//! "shift-F1".

	shortcut(const std::string_view &string);

private:
	//! Internal constructor

	shortcut(size_t p,
		 const std::string_view &string);
public:

	//! Whether a key event matches this shortcut.
	bool matches(const key_event &ke) const;

	//! Description of this shortcut, as a unicode string.

	operator std::u32string() const;
};

//! Inactive shortcut

//! Used in menus to show a shortcut for a menu item but without creating
//! the actual shortcut. The key combination gets implemented elsewhere.
//! This is used for copy/cut/paste menu items. The input field response to
//! the key combinations itself.

class inactive_shortcut : public shortcut {

public:
	using shortcut::shortcut;
};

LIBCXXW_NAMESPACE_END

#endif
