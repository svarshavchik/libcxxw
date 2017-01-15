/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_rectangle_h
#define x_w_rectangle_h

#include <x/w/types.H>
#include <x/w/rectanglefwd.H>

#include <set>

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

	//! Whether two rectangles overlap

	constexpr bool overlaps(const rectangle &o) const
	{
		if (o.x >= x)
		{
			if (o.x-x >= (dim_t::value_type)width)
				return false;
		}
		else
		{
			if (x-o.x >= (dim_t::value_type)o.width)
				return false;
		}
		if (o.y >= y)
		{
			if (o.y-y >= (dim_t::value_type)height)
				return false;
		}
		else
		{
			if (y-o.y >= (dim_t::value_type)o.height)
				return false;
		}

		return true; // No other possibilities.
	}
};

//! Add two rectangle sets together.

//! The returned rectangle set consists of all the combined rectangles, with
//! all overlapping areas removed.

std::set<rectangle> operator+(const std::set<rectangle> &,
			      const std::set<rectangle> &);

//! Return an intersection set of two rectangles.

//! The returned rectangle set consists of only rectangular areas present
//! in both sets.

std::set<rectangle> operator*(const std::set<rectangle> &,
			      const std::set<rectangle> &);

//! Subtract two rectangle sets.

//! The returned rectangle set consists of the first rectangle set, with
//! rectangles in the second set removed.

std::set<rectangle> operator-(const std::set<rectangle> &,
			      const std::set<rectangle> &);

LIBCXXW_NAMESPACE_END

#endif
