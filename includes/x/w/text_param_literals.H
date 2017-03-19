/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_param_literals_H
#define x_w_text_param_literals_H

#include <x/w/theme_font.H>
#include <x/w/theme_color.H>

LIBCXXW_NAMESPACE_START

//! "sans_serif"_font specifies a theme font in a \ref text_param "text parameter".

theme_font operator"" _font(const char *, size_t);

//! "0%"_color specifies a theme font in a \ref text_param "text parameter".
theme_color operator"" _color(const char *, size_t);

LIBCXXW_NAMESPACE_END

using LIBCXXW_NAMESPACE::operator "" _font;
using LIBCXXW_NAMESPACE::operator "" _color;

#endif
