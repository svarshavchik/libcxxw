/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgb_H
#define x_w_rgb_H

#include <x/w/rgbfwd.H>
#include <x/w/namespace.H>
#include <stdlib.h>

LIBCXXW_NAMESPACE_START

//! RGB color, with an alpha channel

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
	rgb() : r(0), g(0), b(0), a(maximum) {}

	//! Constructor
	rgb(rgb_component_t rv, rgb_component_t gv, rgb_component_t bv,
	    rgb_component_t av=maximum) : r(rv), g(gv), b(bv), a(av) {}

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
};

//! A linear gradient for a display element's background color.

//! Define a linear gradient from (x1, y1) to (x2, y2).
//!
//! The x and y coordinates range between 0 and 1. (0, 0) is the display
//! element's top left corner, (1, 1) is its bottom right corner.
//!
//! If fixed_width and fixed_height are not 0, this computes the gradient
//! for a display element with a fixed width or height, rather than the
//! actual height of the display element. fixed_width and fixed_height
//! gets specified in millimeters.

struct linear_gradient {
	double x1=0, y1=0, x2=1, y2=1;
	double fixed_width=0, fixed_height=0;

	rgb_gradient gradient;

	bool operator==(const linear_gradient &o) const noexcept
	{
		return x1 == o.x1 && y1 == o.y1 && x2 == o.x2 && y2 == o.y2 &&
			fixed_width == o.fixed_width &&
			fixed_height == o.fixed_height &&
			gradient == o.gradient;
	}
};

LIBCXXW_NAMESPACE_END

#endif
