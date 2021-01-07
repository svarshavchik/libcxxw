/*
** Copyright 2016-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgbfwd_H
#define x_w_rgbfwd_H

#include <x/w/namespace.H>
#include <x/w/theme_color.H>
#include <x/w/dim_arg.H>
#include <x/w/types.H>
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

//! How to scale an icon.

//! Scale the icon size as close to the requested size as possible,
//! to be at least as big as the requested size, or not bigger than
//! the requested size.

enum class icon_scale { nearest, atleast, nomore};

//! An image-based color specification

struct image_color {

	//! The name of the file

	//! The filename must have a path component, even "./". Without
	//! a path component this loads the image from the current theme
	//!
	//! The image file can be a gif, png, jpg, or the LibCXXw-specific
	//! scalable X graphic image (sxg).
	std::string name;

	//! Repeat the image across the entire drawing area.

	//! Can be used to selectively turn off repeat mode.
	render_repeat repeat=render_repeat::normal;

	//! Scale image to the given width

	//! 0 specifies the original image width. Scaling is supported for
	//! sxg files only.
	dim_arg width=0;

	//! Scale image to the given height

	//! 0 specifies the original image height. Scaling is supported for
	//! sxg files only.
	dim_arg height=0;

	//! Scaling adjustment

	//! Used with sxg files only.

	icon_scale scale=icon_scale::nearest;

	//! Comparison operator
	bool operator==(const image_color &o) const noexcept
	{
		return name == o.name &&
			repeat == o.repeat &&
			width == o.width &&
			height == o.height &&
			scale == o.scale;
	}
};

//! A color specification

//! One of the following:
//!
//! - a \ref rgb "solid color".
//!
//! - the name of a theme-specified color, as a \c std::string
//!
//! - a \ref linear_gradient "linear" or a \ref radial_gradient "radial"
//! gradient color specification.
//!
//! - a \ref image_color "loaded image".

typedef std::variant<rgb, std::string, linear_gradient, radial_gradient,
		     image_color> color_arg;

//! Text label colors

//! Text label colors can only be an explicit rgb value or a theme color

typedef std::variant<theme_color, rgb> text_color_arg;

LIBCXXW_NAMESPACE_END

#endif
