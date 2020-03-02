/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_arg_H
#define x_w_font_arg_H

#include <x/w/font.H>
#include <x/w/theme_font.H>
#include <variant>

LIBCXXW_NAMESPACE_START

//! A font may be specified directly, or as a reference to a theme font

typedef std::variant<font, theme_font> font_arg;

LIBCXXW_NAMESPACE_END

#endif
