/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_mask_h
#define x_w_input_mask_h

#include <string>
#include <x/w/namespace.H>

#include <string_view>

LIBCXXW_NAMESPACE_START

class keysyms;

//! Key and button mask

//! When processing keyboard input, this structure specifies whether any
//! shift/ctrl/etc has been pressed. Also whether any mouse pointer buttons
//! were pressed.

class input_mask {
public:
	//! Indicates that the shift key is pressed
	bool shift=false;

	//! Indicates that the lock key is pressed
	bool lock=false;

	//! Indicates that the control key is pressed
	bool ctrl=false;

	//! Indicates that num lock is on
	bool num_lock=false;

	//! Indicates that the "meta" key is pressed
	bool meta=false;

	//! Indicates that the "alt" key is pressed
	bool alt=false;

	//! Indicates that the "super" key is pressed
	bool super=false;

	//! Indicates that the "hyper" key is pressed
	bool hyper=false;

	//! Indicates that the "mode switch" key is pressed
	bool mode_switch=false;

	//! None of the special keys are pressed
	bool notspecial() const
	{
		return !meta && !alt && !super && !hyper;
	}

	//! Pointer buttons

	//! This is a bitmask. "Button1" is bit 0.
	//!
	//! Must be unsigned type, for std::string operator code.
	unsigned char buttons=0;

	//! Default constructor
	input_mask() noexcept;

	//! Construct from SETofKEYBUTMASK protocol value, and keysyms info

	//! \internal
	input_mask(uint16_t SETofKEYBUTMASK, const keysyms &k) noexcept
		LIBCXX_HIDDEN;

	//! Describe the mask
	operator std::string() const;

	//! Construct the mask from a string

	input_mask(//! mask description

		   //! This is a list of
		   //! "shift", "lock", "ctrl", "meta", "alt", "super",
		   //! "hyper", "mode_switch", and "button#". The delimiter
		   //! separating multiple words can be a comma, whitespace,
		   //! "+", or "-" characters.

		   const std::string_view &s);

	//! If this input mask has a modifier set, so must the other one.

	//! If this input mask does not have a modifier set, the other
	//! input mask may or may not have the same modifier set. The only
	//! requirement, as stated: "if this input mask has a modifier set,
	//! so must the other one."

	bool same_shortcut_modifiers(const input_mask &o) const;

	//! Add up how many modifiers and buttons are set here

	//! If ignore_toggles is set, num_lock and mode_switch do not count.

	int ordinal(bool ignore_toggles=false) const;

	//! Return the protocol SETofKEYBUTMASK value.

	//! \internal
	uint16_t keybuttonmask(const keysyms &k) const LIBCXX_HIDDEN;

	//! Return the protocol SETofKEYMASK value.

	//! \internal
	uint16_t keymask(const keysyms &k) const LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
