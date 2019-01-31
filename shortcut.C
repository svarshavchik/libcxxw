/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/shortcut.H"
#include "x/w/key_event.H"
#include "messages.H"
#include <x/exception.H>
#include <courier-unicode.h>
#include <X11/keysym.h>
#include <cstring>
#include <algorithm>

LIBCXXW_NAMESPACE_START

static const struct {
	uint32_t keysym;
	const char *name;
} special_keys[]={
		  {XK_Left,      "Left"},
		  {XK_KP_Left,   "KP_Left"},
		  {XK_Right,     "Right"},
		  {XK_KP_Right,  "KP_Right"},
		  {XK_Up,        "Up"},
		  {XK_KP_Up,     "KP_Up"},
		  {XK_Down,      "Down"},
		  {XK_KP_Down,   "KP_Down"},
		  {XK_Delete,    "Del"},
		  {XK_KP_Delete, "KP_Del"},
		  {XK_Page_Up,   "PgUp"},
		  {XK_KP_Page_Up,"KP_PgUp"},
		  {XK_Page_Down, "PgDn"},
		  {XK_KP_Page_Down,"KP_PgDn"},
		  {XK_Home,      "Home"},
		  {XK_End,       "End"},
		  {XK_KP_Home,   "KP_Home"},
		  {XK_KP_End,    "KP_End"},
		  {XK_Insert,    "Ins"},
		  {XK_KP_Insert, "KP_Ins"},
};

shortcut::shortcut() : unicode(0), keysym(0)
{
}
shortcut::shortcut(char32_t unicode) : unicode(unicode), keysym(0)
{
}

shortcut::operator bool() const { return unicode != 0 || keysym != 0; }

shortcut::shortcut(const std::string_view &modifier,
		   char32_t unicode)
	: input_mask(modifier), unicode(unicode), keysym(0)
{
}

static inline size_t last_dash(const std::string_view &string)
{
	auto last_dash=string.rfind('-');

	if (last_dash == string.npos)
		last_dash=0;

	return last_dash;
}

shortcut::shortcut(const std::string_view &string)
	: shortcut(last_dash(string), string)
{
}

shortcut::shortcut(size_t dash_pos,
		   const std::string_view &string)
	: input_mask(string.substr(0, dash_pos))
{
	if (string.empty())
		return;

	if (dash_pos < string.size() && string[dash_pos] == '-')
		++dash_pos;

	if (dash_pos < string.size())
	{
		auto key=string.substr(dash_pos);

		auto ustr=unicode::iconvert::tou::convert(std::string{key},
							  unicode::utf_8)
			.first;

		if (ustr.size() == 1)
		{
			unicode=ustr.at(0);
			return;
		}
		if (key[0] == 'f' || key[0] == 'F')
		{
			std::istringstream i{std::string{key.substr(1)}};

			int n;

			i >> n;

			if (!i.fail() && n >= 1 && n <= 35)
			{
				keysym=XK_F1-1+n;
				unicode=0;
				return;
			}
		}

		for (auto &c:ustr)
			c=unicode_lc(c);

		for (const auto &sk:special_keys)
		{
			size_t l=strlen(sk.name);

			char32_t name[l+1];

			std::copy(sk.name, sk.name+l, name);

			name[l]=0;
			for (auto &c:name)
				c=unicode_lc(c);

			if (ustr == name)
			{
				keysym=sk.keysym;
				unicode=0;
				return;
			}
		}
	}
	throw EXCEPTION(_("Invalid shortcut key"));
}

bool shortcut::matches(const key_event &ke) const
{
	// Case-insensitive comparison, for plain characters.

	if (unicode)
	{
		if (unicode_uc(unicode) != ke.unicode &&
		    unicode_lc(unicode) != ke.unicode &&
		    unicode_tc(unicode) != ke.unicode)
			return false;
	}
	else
	{
		// The shortcut is a keysym.

		if (ke.unicode || keysym != ke.keysym)
			return false;
	}

	// Not so fast, the modifiers must match too.

	return same_shortcut_modifiers(ke);
}

shortcut::operator std::u32string() const
{
	std::u32string s;

	if (ordinal())
	{
		auto mask=input_mask::operator std::string();

		s.insert(s.end(), mask.begin(), mask.end());

		s.push_back('-');
	}

	if (unicode)
	{
		s.push_back(unicode);
	}
	else
	{
		for (const auto &sk:special_keys)
		{
			if (keysym==sk.keysym)
			{
				s.insert(s.end(),
					 sk.name,
					 sk.name+strlen(sk.name));
				return s;
			}
		}

		if (keysym < XK_F1 || keysym > XK_F35)
			throw EXCEPTION(_("No description available for the shortcut"));

		std::ostringstream o;

		o << 'F' << (keysym-XK_F1+1);

		std::string f=o.str();

		s.insert(s.end(), f.begin(), f.end());
	}

	return s;
}

LIBCXXW_NAMESPACE_END
