/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/shortcut.H"
#include "x/w/key_event.H"
#include "messages.H"
#include <x/exception.H>
#include <courier-unicode.h>
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

shortcut::shortcut() : unicode(0), keysym(0)
{
}
shortcut::shortcut(char32_t unicode) : unicode(unicode), keysym(0)
{
}

shortcut::operator bool() const { return unicode != 0 || keysym != 0; }

shortcut::shortcut(const std::experimental::string_view &modifier,
		   char32_t unicode)
	: input_mask(modifier), unicode(unicode), keysym(0)
{
}

shortcut::shortcut(const std::experimental::string_view &string)
	: shortcut(string.rfind('-'), string)
{
}

shortcut::shortcut(size_t dash_pos,
		   const std::experimental::string_view &string)
	: input_mask(string.substr(0, dash_pos))
{
	if (dash_pos < string.size() && ++dash_pos < string.size())
	{
		auto key=string.substr(dash_pos);

		if (key[0] == 'f' || key[0] == 'F')
		{
			std::istringstream i{std::string{key.substr(1)}};

			int n;

			i >> n;

			if (!i.fail() && n >= 1 && n <= 35)
			{
				keysym=XK_F1-1+n;
				return;
			}
		}
	}
	throw EXCEPTION(_("Invalid shortcut key"));
}

bool shortcut::matches(const key_event &ke) const
{
	// Must be a keypress.
	if (!ke.keypress)
		return false;

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

LIBCXXW_NAMESPACE_END