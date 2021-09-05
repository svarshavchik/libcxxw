/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_theme_font_H
#define x_w_theme_font_H

#include <string>

LIBCXXW_NAMESPACE_START

//! A distinct type for a name of a font specified by the current theme.

//! Provide a distinct type for a name of a theme-specified font, when
//! creating a \ref text_param "text parameter".
//!
//! The "name" parameter contains the same string that can be passed
//! as a constructor to a \ref font "INSERT_LIBX_NAMESPACE::w::font",
//! except that the font family may also refer to a theme-specified font.

struct theme_font {

	std::string name;

	bool operator==(const theme_font &o) const
	{
		return name == o.name;
	}

	bool operator!=(const theme_font &o) const
	{
		return name != o.name;
	}
};

LIBCXXW_NAMESPACE_END

#endif
