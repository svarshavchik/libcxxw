/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
/*
** Copyright 2016-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgbfwd_H
#define x_w_rgbfwd_H

#include <x/w/namespace.H>
#include <x/w/theme_color.H>
#include <variant>
#include <string>
#include <cstdint>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

class rgb;

//! Value type for RGB components

typedef uint16_t rgb_component_t;

//! Value type that can be used for scaling rgb components.

typedef uint32_t rgb_component_squared_t;

//! A gradient involving multiple colors

//! The map must have a value for key 0. The highest
//! key in the map defines the ending rgb color. The rgb at
//! position \#i defines the rgb color for that value.

typedef std::unordered_map<size_t, rgb> rgb_gradient;

struct linear_gradient;
struct radial_gradient;

//! A color can be specified as either a literal rgb value, or a theme color

//! The background color for a widget may also be specified as a
//! linear or a radial gradient. Gradient colors are ignored in all other
//! situations (border colors, etc...), and fall back to using the #0 gradient
//! color.

typedef std::variant<rgb, std::string, linear_gradient, radial_gradient
		     > color_arg;

//! Text label colors

//! Text label colors can only be an explicit rgb value or a theme color

typedef std::variant<theme_color, rgb> text_color_arg;

LIBCXXW_NAMESPACE_END

#endif
