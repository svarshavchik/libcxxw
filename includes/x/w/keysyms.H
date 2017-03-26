#ifndef keysyms_H
#define keysyms_H

#include <x/logger.H>
#include "x/w/xwindowfwd.H"
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <vector>
#include <array>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

class LIBCXX_HIDDEN keysyms {

	LOG_CLASS_SCOPE;

	class key_sym_to_code;

 public:

	xcb_key_symbols_t *key_symbols;

	keysyms(xcb_connection_t *conn);

	~keysyms();

	// The constructor figures out which modifiers are used for which
	// purpose, on this keyboard. The modifiers are: shift, lock, ctrl,
	// mod1-5. So, if mode_switch is mod1, mode_switch_modifier_mask
	// is 0x08.

	unsigned char mode_switch_modifier_mask,
		num_lock_modifier_mask,
		shift_lock_modifier_mask,
		meta_modifier_mask,
		alt_modifier_mask,
		super_modifier_mask,
		hyper_modifier_mask;

	// Given a keycode, and current modifier mask, calculates which
	// unicode has been pressed (if the key represents a character key),
	// and/or a keysym.
	//
	// Returns 'true' if a character key was pressed, and unicode is
	// initialized to the unicode. Return 'false' if a non-character key
	// was pressed.

	bool lookup(xcb_keycode_t keycode,
		    const input_mask &mask,
		    uint32_t &unicode,
		    uint32_t &keysym) const;

	static bool keysym_to_unicode(uint32_t keysym, uint32_t &unicode);

	std::vector<xcb_keycode_t> get_keycodes_for_keysym(uint32_t keysym)
		const;

	static void get_modifier_mappings(xcb_connection_t *conn,
					  std::array<std::vector<xcb_keycode_t>,
					  8> &modifiers);
};

#if 0
{
	{
#endif
	}
}
#endif
