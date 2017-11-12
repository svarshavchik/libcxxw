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
#include <map>

LIBCXXW_NAMESPACE_START

class rgb;

//! A gradient involving multiple colors

//! The std::map must have a value for key 0. The highest
//! key in the map defines the ending rgb color. The rgb at
//! position \#i defines the rgb color for that value.

typedef std::map<unsigned, rgb> rgb_gradient;

//! A color can be specified as either a literal rgb value, or a theme color

typedef std::variant<std::string, rgb> color_arg;

//! A gradient can be specified either literally, or as a theme graident.

typedef std::variant<std::string, rgb_gradient> rgb_gradient_arg;

LIBCXXW_NAMESPACE_END

#endif
