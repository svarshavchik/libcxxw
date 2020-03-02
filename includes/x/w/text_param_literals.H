/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_param_literals_H
#define x_w_text_param_literals_H

#include <x/w/theme_font.H>
#include <x/w/theme_color.H>

LIBCXXW_NAMESPACE_START

enum class text_decoration;

//! Theme font name

//! "sans_serif"_theme_font specifies a theme font in a \ref text_param "text parameter".
//!
//! See \ref font_decorator "_font literal" for more information on font
//! literals.

theme_font operator"" _theme_font(const char *, size_t);

//! Theme color name

//! "0%"_color specifies a theme font in a \ref text_param "text parameter".
theme_color operator"" _color(const char *, size_t);

//! Text decorations

//! "underline"_decoration and "no"_decoration are the only two possible values.

text_decoration operator""_decoration(const char *, size_t);

LIBCXXW_NAMESPACE_END

using LIBCXXW_NAMESPACE::operator "" _theme_font;
using LIBCXXW_NAMESPACE::operator "" _color;
using LIBCXXW_NAMESPACE::operator "" _decoration;

#endif
