/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgb_H
#define x_w_rgb_H

#include <x/w/rgbfwd.H>
#include <x/w/namespace.H>
#include <cstdint>
#include <map>
#include <stdlib.h>

LIBCXXW_NAMESPACE_START

//! RGB color, with an alpha channel

class rgb {

public:
	//! Type of each component

	typedef uint16_t component_t;

	//! Red component
	component_t r;

	//! Green component
	component_t g;

	//! Blue component
	component_t b;

	//! Alpha component
	component_t a;

	//! Maximum intensity

	static constexpr component_t maximum=0xFFFF;

	//! Default constructor
	rgb() : r(0), g(0), b(0), a(maximum) {}

	//! Constructor
	rgb(component_t rv, component_t gv, component_t bv,
	    component_t av=maximum) : r(rv), g(gv), b(bv), a(av) {}

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

	//! Create a gradient of colors

	//! Returns the ith color in a gradient of n colors from this color
	//! to another color.
	//!
	//! When i=0 returns this color.
	//! When i=n returns other.
	rgb gradient(const rgb &other, component_t i, component_t n) const;

	//! A gradient involving multiple colors

	//! The std::map must have a value for key 0. The highest
	//! key in the map defines the ending rgb color. The rgb at
	//! position \#i defines the rgb color for that value.

	typedef std::map<unsigned, rgb> gradient_t;

	//! Compute multiple gradient.

	//! Returns the ith color in a gradient of n colors across a gradient
	//! color range defined by a gradient_t.
	//!
	//! When i=0 returns gradient_t[0].
	//! When i=n returns gradient_t[k] where k is the maximum value in
	//! gradient_t.

	static rgb gradient(const gradient_t &g, size_t i, size_t n);
};

LIBCXXW_NAMESPACE_END

#endif
