/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgbfwd_H
#define x_w_rgbfwd_H

#include <x/w/namespace.H>
#include <variant>
#include <string>

LIBCXXW_NAMESPACE_START

class rgb;

//! A color can be specified as either a literal rgb value, or a theme color

typedef std::variant<std::string, rgb> color_arg;

LIBCXXW_NAMESPACE_END

#endif
