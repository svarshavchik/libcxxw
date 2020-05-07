/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_shortcut_h
#define x_w_shortcut_h

#include <string>
#include <utility>
#include <x/w/shortcutfwd.H>
#include <x/w/input_mask.H>
#include <x/w/key_eventfwd.H>

LIBCXXW_NAMESPACE_START

//! A keyboard shortcut for a \ref hotspot "hotspot".

//! Defines a keyboard shortcut for activating a hotspot no matter which
//! widget has keyboard focus.

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

	//! Internal helper

	struct shortcut_parse_info {

		//! Shortcut prefix
		std::string_view prefix;

		//! Shortcut key
		std::string_view key;

		//! Constructor
		shortcut_parse_info(const std::string_view &);

		//! Delete copy constructor
		shortcut_parse_info(const shortcut_parse_info &)=delete;
	};

	//! Specify a shortcut as a single string.

	//! The constructor's parameter must be one of the following:
	//!
	//! - a single (UTF-8) character
	//!
	//! - "Fn", specifying a function key.
	//!
	//! - "Left", "Right", "Up", "Down", "Del", "Ins", "PgUp", "PgDn",
	//!   "Home", "End", with an optional "Kp_" prefix.
	//!
	//! This sets the unicode shortcut to the character, or the keysym
	//! to the appropriate function key or special key.
	//!
	//! The specified string may also have a "modifier-" prefix, example:
	//! "shift-F1".
	//!
	//! The specified string may also have a leading "${  }" prefix,
	//! such as:
	//!
	//! "${context:creator}Alt-F"
	//!
	//! This prefix gets stripped off, and ignored. This can be used with
	//! gettext to provide localization of different shortcut keys that
	//! have the same localized value, such as
	//!
	//! "${context:file_menu}Alt-F"  (for "File")
	//!
	//! and
	//!
	//! "${context:edit_menu}Alt-F"  (for "Find")
	//!
	//! Localized "File" and "Find" strings may then have different
	//! localized shortcuts.

	template<typename T,
		 typename=decltype(shortcut_parse_info{std::declval<T>()})>
	shortcut(T && string)
		: shortcut{shortcut_parse_info{std::forward<T>(string)}}
	{
	}

	//! Standard shortcut keysym

	//! If the parameter is one of the KP_ keysyms mentioned by the
	//! constructor, return the garden-variety keysym. If it's the garden
	//! variety keysym, return the KP_ keysym. If there is no KP_
	//! keysym, return the smae keysym.

	static uint32_t shortcut_keysym(uint32_t);
private:

	//! Internal constructor

	shortcut(shortcut_parse_info);
public:

	//! Whether a key event matches this shortcut.
	bool matches(const key_event &ke) const;

	//! Description of this shortcut, as a string.

	std::string description() const;

	//! Description of this shortcut, as a unicode label.

	//! This is used to label this shortcut in menus.
	std::u32string label() const;
};

//! Inactive shortcut

//! Used in menus to show a shortcut for a menu item but without creating
//! the actual shortcut. The key combination gets implemented elsewhere.
//! This is used for copy/cut/paste menu items. The input field response to
//! the key combinations itself.

class inactive_shortcut : public shortcut {

public:
	using shortcut::shortcut;

	//! Constructor
	inline inactive_shortcut(const shortcut &sc)
		: shortcut{sc}
	{
	}

	//! Constructor
	inline inactive_shortcut(shortcut &&sc)
		: shortcut{std::move(sc)}
	{
	}

};

LIBCXXW_NAMESPACE_END

#endif
