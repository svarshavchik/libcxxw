/*
** Copyright 2017-2020 Double Precision, Inc.
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
#include <charconv>

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

uint32_t shortcut::shortcut_keysym(uint32_t n)
{
	switch (n) {
	case XK_KP_Left:   return XK_Left;
	case XK_KP_Right:  return XK_Right;
	case XK_KP_Up:     return XK_Up;
	case XK_KP_Down:   return XK_Down;
	case XK_KP_Delete: return XK_Delete;
	case XK_KP_Page_Up:return XK_Page_Up;
	case XK_KP_Page_Down:return XK_Page_Down;
	case XK_KP_Home:   return XK_Home;
	case XK_KP_End:    return XK_End;
	case XK_KP_Insert: return XK_Insert;
	default:
		break;
	}
	return n;
}

shortcut::operator bool() const { return unicode != 0 || keysym != 0; }

shortcut::shortcut(const std::string_view &modifier,
		   char32_t unicode)
	: input_mask(modifier), unicode(unicode), keysym(0)
{
}

shortcut::shortcut_parse_info
::shortcut_parse_info(const std::string_view &string)
	: key{string}
{
	// gettext label:
	const char *p=key.begin();
	const char *q=key.end();

	if (p != q && *p =='$' && ++p != q && *p == '{')
	{
		while (p != q)
			if (*p++ == '}')
			{
				key=std::string_view{p,
						     static_cast<size_t>(q-p)};
				break;
			}
	}

	auto last_dash=key.rfind('-');

	if (last_dash != key.npos)
	{
		if (last_dash + 1 < key.size())
		{
			prefix=key.substr(0, last_dash);
			key=key.substr(++last_dash);
		}
	}
}

shortcut::shortcut(shortcut_parse_info info)
	: input_mask{info.prefix}
{
	if (info.prefix.empty() && info.key.empty())
		return;

	if (!info.key.empty())
	{
		auto ustr=unicode::iconvert::tou::convert(std::string{info.key},
							  unicode::utf_8)
			.first;

		if (ustr.size() == 1 && ordinal())
		{
			// Require a modifier for a single key.
			unicode=ustr.at(0);
			return;
		}

		if (info.key[0] == 'f' || info.key[0] == 'F')
		{
			const char *p=info.key.begin();

			++p;

			const char *q=info.key.end();

			int n;

			auto ret=std::from_chars(p, q, n);

			if (ret.ec == std::errc{} &&
			    n >= 1 && n <= 35)
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

		char fkey[8]={'F'};

		auto ret=std::to_chars(fkey+1, fkey+6, (keysym-XK_F1+1));

		if (ret.ec != std::errc{})
			throw EXCEPTION(_("Cannot convert FK number"));

		s.insert(s.end(), fkey, ret.ptr);
	}

	return s;
}

LIBCXXW_NAMESPACE_END
