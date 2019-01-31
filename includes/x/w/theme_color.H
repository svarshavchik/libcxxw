/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_theme_color_H
#define x_w_theme_color_H

#include <string>

LIBCXXW_NAMESPACE_START

//! A distinct type for a name of a color specified by the current theme.

//! Provide a distinct type for a name of a theme-specified color, when
//! creating a \ref text_param "text parameter".

struct theme_color {

	std::string name;
};

LIBCXXW_NAMESPACE_END

#endif
