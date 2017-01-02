/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_rectangle_h
#define x_w_rectangle_h

#include <x/w/types.H>
#include <x/w/rectanglefwd.H>
#include <iostream>

LIBCXXW_NAMESPACE_START

//! A rectangle, pretty much it.

struct rectangle {

	//! Coordinate
	coord_t x=coord_t(0);
	//! Coordinate
	coord_t y=coord_t(0);
	//! Coordinate
	dim_t width=dim_t(0);
	//! Coordinate
	dim_t height=dim_t(0);

	//! Destructor
	~rectangle()=default;

	//! Strict weak ordering
	constexpr bool operator<(const rectangle &o) const
	{
		return x < o.x ||
			 (x == o.x &&
			  (y < o.y ||
			   (y == o.y &&
			  (width < o.width ||
			   (width == o.width && height < o.height)))));
	}

	//! Equality operator
	constexpr bool operator==(const rectangle &o) const
	{
		return x == o.x &&
			y == o.y &&
			width == o.width &&
			height == o.height;
	}

	//! Inequality operator
	constexpr bool operator!=(const rectangle &o) const
	{
		return !operator==(o);
	}
};

LIBCXXW_NAMESPACE_END

#endif
