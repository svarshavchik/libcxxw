/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgb_H
#define x_w_rgb_H

#include <x/w/rgbfwd.H>
#include <x/w/namespace.H>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <iostream>

LIBCXXW_NAMESPACE_START

//! RGB color, with an alpha channel

//! \see transparent
//! \see black
//! \see gray
//! \see silver
//! \see white
//! \see maroon
//! \see red
//! \see olive
//! \see yellow
//! \see green
//! \see lime
//! \see teal
//! \see aqua
//! \see navy

class rgb {

public:
	//! Red component
	rgb_component_t r;

	//! Green component
	rgb_component_t g;

	//! Blue component
	rgb_component_t b;

	//! Alpha component
	rgb_component_t a;

	//! Maximum intensity

	static constexpr rgb_component_t maximum=0xFFFF;

	//! Default constructor
	constexpr rgb() : r{0}, g{0}, b{0}, a{maximum} {}

	//! Constructor
	constexpr rgb(rgb_component_t rv, rgb_component_t gv,
		      rgb_component_t bv,
		      rgb_component_t av=maximum)
	: r{rv}, g{gv}, b{bv}, a{av} {}

	//! Comparator

	bool operator==(const rgb &o) const
	{
		return r == o.r && g == o.g && b == o.b && a == o.a;
	}

	//! Comparator
	bool operator!=(const rgb &o) const
	{
		return !operator==(o);
	}

	//! Comparator, for associative containers

	bool operator<(const rgb &a) const
	{
		return value() < a.value();
	}

	//! Comparator, for associative containers

	uint64_t value() const noexcept
	{
		return (((((((uint64_t)a) << 16) | r) << 16) | g) << 16) | b;
	}

	//! Convert to a readable label
	operator std::string() const;

	//! Convert from a string
	rgb(const std::string_view &);
};

//! Transparent color
constexpr rgb transparent{0, 0, 0, 0};

//! Standard HTML 3.2 color
constexpr rgb black{0, 0, 0};
//! Standard HTML 3.2 color
constexpr rgb gray{rgb::maximum/2+1, rgb::maximum/2+1, rgb::maximum/2+1};
//! Standard HTML 3.2 color
constexpr rgb silver{(rgb::maximum/4+1)*3, (rgb::maximum/4+1)*3,
		(rgb::maximum/4+1)*3};
//! Standard HTML 3.2 color
constexpr rgb white{rgb::maximum, rgb::maximum, rgb::maximum};

//! Standard HTML 3.2 color
constexpr rgb maroon{rgb::maximum/2+1, 0, 0};
//! Standard HTML 3.2 color
constexpr rgb red{rgb::maximum, 0, 0};
//! Standard HTML 3.2 color
constexpr rgb olive{rgb::maximum/2+1, rgb::maximum/2+1, 0};
//! Standard HTML 3.2 color
constexpr rgb yellow{rgb::maximum, rgb::maximum, 0};

//! Standard HTML 3.2 color
constexpr rgb green{0, rgb::maximum/2+1, 0};
//! Standard HTML 3.2 color
constexpr rgb lime{0, rgb::maximum, 0};
//! Standard HTML 3.2 color
constexpr rgb teal{0, rgb::maximum/2+1, rgb::maximum/2+1};
//! Standard HTML 3.2 color
constexpr rgb aqua{0, rgb::maximum, rgb::maximum};

//! Standard HTML 3.2 color
constexpr rgb navy{0, 0, rgb::maximum/2+1};
//! Standard HTML 3.2 color
constexpr rgb blue{0, 0, rgb::maximum};
//! Standard HTML 3.2 color
constexpr rgb fuchsia{rgb::maximum, 0, rgb::maximum};
//! Standard HTML 3.2 color
constexpr rgb purple{rgb::maximum/2+1, 0, rgb::maximum/2+1};

//! Overload << operator

std::ostream &operator<<(std::ostream &, const rgb &);

//! Values of a \ref linear_gradient "linear gradient".

struct linear_gradient_values {
	double x1=0, y1=0, x2=1, y2=1;
	double fixed_width=0, fixed_height=0;

	//! Comparison operator
	inline bool operator==(const linear_gradient_values &o) const
	{
		return x1 == o.x1 && y1 == o.y1 &&
			x2 == o.x2 && y2 == o.y2 &&
			fixed_width == o.fixed_width &&
			fixed_height == o.fixed_height;
	}

	//! Comparison operator
	inline bool operator!=(const linear_gradient_values &o) const
	{
		return !operator==(o);
	}
};

//! A linear gradient for a widget's background color.

//! Define a linear gradient from (x1, y1) to (x2, y2). Closely modeled on
//! the RENDER extension's CreateLinearGradient request, because that's what
//! it is.
//!
//! The x and y coordinates range between 0 and 1. (0, 0) is the display
//! element's top-left corner, (1, 1) is its bottom-right corner.
//!
//! If fixed_width and fixed_height are not 0, this computes the gradient
//! for a widget with a fixed width or height, rather than the
//! actual size of the widget. fixed_width and fixed_height
//! gets specified in millimeters. The (0, 0) coordinate is still the
//! widget's top-left corner, and (1, 1) now being wherever it is,
//! if the widget's size is as specified.
//! Negative fixed width and height anchors
//! the virtual (0, 0)-(1, 1) coordinate space to the opposite corner.

struct linear_gradient : linear_gradient_values {

	rgb_gradient gradient;
};

//! Values of a \ref radial_gradient "radial gradient".

struct radial_gradient_values {

	double inner_center_x=.5,
		inner_center_y=.5,
		outer_center_x=.5,
		outer_center_y=.5,

		inner_radius=0,
		outer_radius=.5;

	enum radius_axis {
			  //! The widget's width
			  horizontal,

			  //! The widget's height
			  vertical,

			  //! The widget's width or height, whichever's the shortest
			  shortest,

			  //! The widget's width or height, whichever's the longest
			  longest
	};

	radius_axis inner_radius_axis=shortest;
	radius_axis outer_radius_axis=longest;

	double fixed_width=0, fixed_height=0;

	//! Comparison operator
	inline bool operator==(const radial_gradient_values &o) const
	{
		return inner_center_x == o.inner_center_x &&
			inner_center_y == o.inner_center_y &&
			outer_center_x == o.outer_center_x &&
			outer_center_y == o.outer_center_y &&
			inner_radius == o.inner_radius &&
			outer_radius == o.outer_radius &&
			inner_radius_axis == o.inner_radius_axis &&
			outer_radius_axis == o.outer_radius_axis &&
			fixed_width == o.fixed_width &&
			fixed_height == o.fixed_height;
	}

	//! Comparison operator
	inline bool operator!=(const radial_gradient_values &o) const
	{
		return !operator==(o);
	}
};

//! A radial gradient for a widget's background color.

//! Define a radial gradient from an inner circle to an outer circle.
//! Closely modeled on
//! the RENDER extension's CreateRadialGradient request, because that's what
//! it is.
//!
//! The x and y coordinates range between 0 and 1. (0, 0) is the display
//! element's top left corner, (1, 1) is its bottom right corner.
//!
//! inner_radius and outer_radius gives the radius of the inner and outer
//! circles.
//!
//! inner_radius_axis and outer_radius_axis specify whether the respective
//! radius is based on the widget's width or height:
//!
//! - horizontal: this radius axis is based on the widget's width
//!
//! - vertical: this radius axis is based on the widget's height
//!
//! - shortest: whichever one's shortest
//!
//! - longest: whichever one's shortest
//!
//! If fixed_width and fixed_height are not 0, this computes the gradient
//! for a widget with a fixed width or height, rather than the
//! actual size of the widget. fixed_width and fixed_height
//! gets specified in millimeters. The (0, 0) coordinate is still the
//! widget's top-left corner, and (1, 1) now being wherever it is,
//! if the widget's size is as specified.
//! Negative fixed width and height anchors
//! the virtual (0, 0)-(1, 1) coordinate space to the opposite corner.

struct radial_gradient : radial_gradient_values {

	rgb_gradient gradient;

	bool operator==(const radial_gradient &o) const noexcept
	{
		return inner_center_x == o.inner_center_x &&
			inner_center_y == o.inner_center_y &&
			outer_center_x == o.outer_center_x &&
			outer_center_y == o.outer_center_y &&
			inner_radius == o.inner_radius &&
			outer_radius == o.outer_radius &&
			inner_radius_axis == o.inner_radius_axis &&
			outer_radius_axis == o.outer_radius_axis &&
			fixed_width == o.fixed_width &&
			fixed_height == o.fixed_height &&
			gradient == o.gradient;
	}
};

LIBCXXW_NAMESPACE_END

#endif
