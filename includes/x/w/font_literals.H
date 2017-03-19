/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_literals_H
#define x_w_font_literals_H

#include <x/w/font.H>

LIBCXXW_NAMESPACE_START

//! "liberation mono;point_size=10"_font constructs a \ref font "font".

font operator"" _font(const char *, size_t);

LIBCXXW_NAMESPACE_END

using LIBCXXW_NAMESPACE::operator "" _font;

#endif
