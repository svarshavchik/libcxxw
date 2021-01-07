/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/shortcut.H"
#include "x/w/key_event.H"
#include "messages.H"
#include <x/exception.H>
#include <x/visitor.H>
#include <courier-unicode.h>
#include <X11/keysym.h>
#include <cstring>
#include <algorithm>
#include <functional>
#include <charconv>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

static const struct special_keys_t {
	uint32_t keysym;
	char name[10];
	char32_t usym[8];
} special_keys[]={
		  // label() relies on the KP_ naming convention.

		  {XK_Left,	 "Left",	U"\x2B05"},
		  {XK_KP_Left,	 "KP_Left",	U"KP-\x2B05"},
		  {XK_Right,	 "Right",	U"\x2B95"},
		  {XK_KP_Right,	 "KP_Right",	U"KP-\x2B95"},
		  {XK_Up,	 "Up",		U"\x2B06"},
		  {XK_KP_Up,	 "KP_Up",	U"KP-\x2B06"},
		  {XK_Down,	 "Down",	U"\x2B07"},
		  {XK_KP_Down,   "KP_Down",	U"KP-\x2B07"},
		  {XK_Delete,    "Del",		U"Del"},
		  {XK_KP_Delete, "KP_Del",	U"KP-Del"},
		  {XK_Page_Up,   "PgUp",	U"PgUp"},
		  {XK_KP_Page_Up,"KP_PgUp",	U"KP-PgUp"},
		  {XK_Page_Down, "PgDn",	U"PgDn"},
		  {XK_KP_Page_Down,"KP_PgDn",	U"KP-PgDn"},
		  {XK_Home,      "Home",	U"Home"},
		  {XK_End,       "End",		U"End"},
		  {XK_KP_Home,   "KP_Home",	U"KP-Home"},
		  {XK_KP_End,    "KP_End",	U"KP-End"},
		  {XK_Insert,    "Ins",		U"Ins"},
		  {XK_KP_Insert, "KP_Ins",	U"KP-Ins"},
};

#if 0
{
#endif
}

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

	case XK_Left:   return XK_KP_Left;
	case XK_Right:  return XK_KP_Right;
	case XK_Up:     return XK_KP_Up;
	case XK_Down:   return XK_KP_Down;
	case XK_Delete: return XK_KP_Delete;
	case XK_Page_Up:return XK_KP_Page_Up;
	case XK_Page_Down:return XK_KP_Page_Down;
	case XK_Home:   return XK_KP_Home;
	case XK_End:    return XK_KP_End;
	case XK_Insert: return XK_KP_Insert;

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

		if (ustr == U"esc")
		{
			unicode='\e';
			return;
		}

		if (ustr == U"enter")
		{
			unicode='\n';
			return;
		}

		if (ustr == U"tab")
		{
			unicode='\t';
			return;
		}

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

namespace {
#if 0
}
#endif

// Common logic for description() and label()

struct description_or_label {

	std::string s;

	std::variant<std::monostate,
		     char32_t,
		     const special_keys_t *> result=std::monostate{};

	description_or_label(const shortcut &sc);
};

description_or_label::description_or_label(const shortcut &sc)
{
	if (sc.ordinal())
	{
		const input_mask &mask=sc;

		s=mask;
		s.push_back('-');
	}

	if (sc.unicode)
	{
		if (sc.unicode == '\e')
			s += "Esc";
		else if (sc.unicode == '\n')
			s += "Enter";
		else if (sc.unicode == '\t')
			s += "Tab";
		else
		{
			result.emplace<char32_t>(sc.unicode);
		}
	}
	else
	{
		for (const auto &sk:special_keys)
		{
			if (sc.keysym==sk.keysym)
			{
				result.emplace<const special_keys_t *>(&sk);
				return;
			}
		}

		char buf[10];

		buf[0]='F';

		auto n=sc.keysym;

		if (n < XK_F1 || n > XK_F35)
		{
			buf[0]='#';
		}
		else
		{
			n = n-XK_F1+1;
		}

		auto ret=std::to_chars(buf+1, buf+sizeof(buf), n);

		if (ret.ec != std::errc{})
			throw EXCEPTION(_("Cannot convert keysym number"));

		s += std::string{buf, ret.ptr};
	}
}

#if 0
{
#endif
}

std::string shortcut::description() const
{
	description_or_label dl{*this};

	std::visit(visitor{
			[&](std::monostate)
			{
			},
			[&](char32_t v)
			{
				char32_t c[2];

				c[0]=v;
				c[1]=0;

				dl.s += unicode::iconvert::fromu
					::convert(c,unicode::utf_8)
					.first;
			},
			[&](const special_keys_t *sk)
			{
				dl.s += sk->name;
			}}, dl.result);

	return dl.s;
}

std::u32string shortcut::label() const
{
	description_or_label dl{*this};

	return std::visit(visitor{
				  [&](std::monostate) -> std::u32string
				  {
					  return {dl.s.begin(),
						  dl.s.end()};
				  },
				  [&](char32_t v)
				  {
					  std::u32string s;

					  s.reserve(dl.s.size()+1);

					  s.insert(s.end(), dl.s.begin(),
						   dl.s.end());

					  s += v;

					  return s;
				  },
				  [&](const special_keys_t *sk)
				  {
					  std::u32string s;
					  std::u32string ss{sk->usym};

					  s.reserve(dl.s.size()+ss.size());

					  s.insert(s.end(), dl.s.begin(),
						   dl.s.end());
					  s += ss;

					  return s;
				  }}, dl.result);
}

LIBCXXW_NAMESPACE_END
