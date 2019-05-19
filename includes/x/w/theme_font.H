/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_theme_font_H
#define x_w_theme_font_H

#include <string>

LIBCXXW_NAMESPACE_START

//! A distinct type for a name of a font specified by the current theme.

//! Provide a distinct type for a name of a theme-specified font, when
//! creating a \ref text_param "text parameter".

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
