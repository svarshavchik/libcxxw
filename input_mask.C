/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/input_mask.H"
#include "keysyms.H"
#include <x/strtok.H>
#include <x/chrcasecmp.H>
#include <xcb/xproto.h>
#include <charconv>
#include "messages.H"

LIBCXXW_NAMESPACE_START

input_mask::input_mask() noexcept
{
}

input_mask::input_mask(uint16_t SETofKEYBUTMASK,
		       const keysyms &k) noexcept
	: shift((SETofKEYBUTMASK & XCB_MOD_MASK_SHIFT) != 0),
	  lock((SETofKEYBUTMASK & XCB_MOD_MASK_LOCK) != 0),
	  ctrl((SETofKEYBUTMASK & XCB_MOD_MASK_CONTROL) != 0),
	  num_lock((SETofKEYBUTMASK & k.num_lock_modifier_mask) != 0),
	  meta((SETofKEYBUTMASK & k.meta_modifier_mask) != 0),
	  alt((SETofKEYBUTMASK & k.alt_modifier_mask) != 0),
	  super((SETofKEYBUTMASK & k.super_modifier_mask) != 0),
	  hyper((SETofKEYBUTMASK & k.hyper_modifier_mask) != 0),
	  mode_switch((SETofKEYBUTMASK & k.mode_switch_modifier_mask) != 0),
	  buttons((SETofKEYBUTMASK >> 8) & 0x1f)
{
}

uint16_t input_mask::keybuttonmask(const keysyms &k) const
{
	return (
		(shift ? XCB_MOD_MASK_SHIFT:0) |
		(lock ? XCB_MOD_MASK_LOCK:0) |
		(ctrl ? XCB_MOD_MASK_CONTROL:0) |
		(num_lock ? k.num_lock_modifier_mask:0) |
		(meta ? k.meta_modifier_mask:0) |
		(alt ? k.alt_modifier_mask:0) |
		(super ? k.super_modifier_mask:0) |
		(hyper ?k.hyper_modifier_mask:0) |
		(mode_switch ? k.mode_switch_modifier_mask:0) |
		((uint16_t)buttons << 8));
}

uint16_t input_mask::keymask(const keysyms &k) const
{
	return keybuttonmask(k) & 0xFF;
}

input_mask::operator std::string() const
{
	std::ostringstream o;
	const char *sep="";

	if (shift)
	{
		o << sep << "Shift";
		sep=", ";
	}

	if (lock)
	{
		o << sep << "Lock";
		sep=", ";
	}

	if (ctrl)
	{
		o << sep << "Ctrl";
		sep=", ";
	}

	if (num_lock)
	{
		o << sep << "Num_Lock";
		sep=", ";
	}
	if (alt)
	{
		o << sep << "Alt";
		sep=", ";
	}
	if (meta)
	{
		o << sep << "Meta";
		sep=", ";
	}
	if (super)
	{
		o << sep << "Super";
		sep=", ";
	}
	if (hyper)
	{
		o << sep << "Hyper";
		sep=", ";
	}

	if (mode_switch)
	{
		o << sep << "Mode_switch";
		sep=", ";
	}

	auto b=buttons;
	int n=0;

	while (b)
	{
		++n;
		if (b & 1)
		{
			o << sep << "Button" << n;
			sep=", ";
		}
		b >>= 1;
	}

	if (!*sep)
		o << "none";

	return o.str();
}

input_mask::input_mask(const std::string_view &o)
{
	std::string_view o_copy{o};

	static const struct {
		char name[12];
		bool input_mask::*field;
	} fields[]={
		{"shift", &input_mask::shift},
		{"lock", &input_mask::lock},
		{"ctrl", &input_mask::ctrl},
		{"num_lock", &input_mask::num_lock},
		{"meta", &input_mask::meta},
		{"alt", &input_mask::alt},
		{"super", &input_mask::super},
		{"hyper", &input_mask::hyper},
		{"mode_switch", &input_mask::mode_switch},
	};

	while (!o_copy.empty())
	{
		size_t n=o_copy.find_first_of(",-+ \r\t\n");

		if (n == 0)
		{
			o_copy.remove_prefix(1);
			continue;
		}

		if (n > o_copy.size())
			n=o_copy.size();

		auto orig_n=n;

		if (n > 32)
			n=32;
		char w_buf[n];

		const char *p=o_copy.data();

		std::copy(p, p+n, w_buf);

		std::transform(w_buf, w_buf+n, w_buf,
			       chrcasecmp::tolower);

		std::string_view w{w_buf, n};

		o_copy.remove_prefix(orig_n);

		bool found=false;
		for (const auto &field:fields)
		{
			if (w == field.name)
			{
				(this->*(field.field))=true;
				found=true;
				break;
			}
		}

		if (found)
			continue;

		if (w.substr(0, 6) == "button")
		{
			unsigned n;

			auto n_str=w.substr(6);

			if (!n_str.empty())
			{
				const char *b=&*n_str.begin();
				const char *e=b+n_str.size();

				auto conv=std::from_chars(b, e, n);

				if (conv.ec==std::errc{} &&
				    conv.ptr==e)
				{
					buttons |= (1 << n);
					continue;
				}
			}
		}

		throw EXCEPTION(_("Invalid input mask"));
	}
}

bool input_mask::same_shortcut_modifiers(const input_mask &o) const
{
	return (!shift || o.shift) &&
		(!lock || o.lock) &&
		(!ctrl || o.ctrl) &&
		(!num_lock || o.num_lock) &&
		(!meta || o.meta) &&
		(!alt || o.alt) &&
		(!super || o.super) &&
		(!hyper || o.hyper) &&
		(!mode_switch || o.mode_switch) &&
		((buttons & o.buttons) ^ buttons) == 0;
}

int input_mask::ordinal(bool ignore_toggles) const
{
	int toggle_value=ignore_toggles ? 0:1;

	int n=(shift ? 1:0) +
		(lock ? 1:0) +
		(ctrl ? 1:0) +
		(num_lock ? toggle_value:0) +
		(meta ? 1:0) +
		(alt ? 1:0) +
		(super ? 1:0) +
		(hyper ? 1:0) +
		(mode_switch ? toggle_value:0);

	auto b=buttons;

	while (b)
	{
		if (b & 1) ++n;

		b >>= 1;
	}
	return n;
}

LIBCXXW_NAMESPACE_END
