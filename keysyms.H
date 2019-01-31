/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef keysyms_H
#define keysyms_H

#include <x/logger.H>
#include "x/w/input_mask.H"
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <vector>
#include <array>
#include <string>

LIBCXXW_NAMESPACE_START

//! The deep voodoo of translating X keyboard symbols to unicode.

class LIBCXX_HIDDEN keysyms {

	LOG_CLASS_SCOPE;

	class key_sym_to_code;

 public:

	//! Loaded in the constructor, and cached.

	xcb_key_symbols_t *key_symbols;

	//! Constructor
	keysyms(xcb_connection_t *conn);

	//! Destructor
	~keysyms();

	keysyms(const keysyms &)=delete;

	keysyms &operator=(const keysyms &)=delete;

	//! Map modifiers to a bitmask.

	//! The constructor figures out which modifiers are used for which
	//! purpose, on this keyboard. The modifiers are: shift, lock, ctrl,
	//! mod1-5. So, if mode_switch is mod1, mode_switch_modifier_mask
	//! is 0x08.

	unsigned char mode_switch_modifier_mask,
		num_lock_modifier_mask,
		shift_lock_modifier_mask,
		meta_modifier_mask,
		alt_modifier_mask,
		super_modifier_mask,
		hyper_modifier_mask;

	//! Look up unicode character given a keycode, and a mask.

	//! Given a keycode, and current modifier mask, calculates which
	//! unicode has been pressed (if the key represents a character key),
	//! and/or a keysym.
	//!
	//! Returns 'true' if a character key was pressed, and unicode is
	//! initialized to the unicode. Return 'false' if a non-character key
	//! was pressed.

	bool lookup(xcb_keycode_t keycode,
		    const input_mask &mask,
		    char32_t &unicode,
		    uint32_t &keysym) const;

	//! Check fixed mappings of keysyms to unicode characters.

	//! Some keysyms, like XK_KP_Space can be mapped to a unicode character,
	//! such as ' '.
	//!
	//! Given a keysym check if it has a fixed unicode character.

	static bool keysym_to_unicode(uint32_t keysym, char32_t &unicode);

	//! Translate a keysym back into a list of keycodes. that can generate it.

	std::vector<xcb_keycode_t> get_keycodes_for_keysym(uint32_t keysym)
		const;

 private:
	//! Internal function used by the constructor.

	static void get_modifier_mappings(xcb_connection_t *conn,
					  std::array<std::vector<xcb_keycode_t>,
					  8> &modifiers);
};

LIBCXXW_NAMESPACE_END
#endif
