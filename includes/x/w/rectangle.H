/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_rectangle_h
#define x_w_rectangle_h

#include <x/w/types.H>
#include <x/w/rectanglefwd.H>

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

	//! Define a class method that returns a side
	constexpr coord_t left() const { return x; }

	//! Define a class method that returns a side
	constexpr coord_t right() const { return coord_t::truncate(x+width); }

	//! Define a class method that returns a side
	constexpr coord_t top() const { return y; }

	//! Define a class method that returns a side
	constexpr coord_t bottom() const { return coord_t::truncate(y+height); }

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

	//! Whether the given X coordinate is inside the rectangle.

	constexpr bool overlaps(coord_t x, coord_t y) const
	{
		return x >= this->x &&
			y >= this->y &&
			x < coord_t::truncate(this->x+width) &&
			y < coord_t::truncate(this->y+height);
	}
};

//! A hash for rectangle structures.

class rectangle_hash {

public:

	inline constexpr size_t operator()(const rectangle &r) const
	{
		return (((size_t)(coord_t::value_type)r.x << 16)
			| ((coord_t::value_type)r.y & 0xFFFF)) ^
			(((size_t)(dim_t::value_type)r.width << 16)
			 | ((dim_t::value_type)r.height & 0xFFFF));
	}
};

//! Add two rectangle sets together.

//! The returned rectangle set consists of all the combined rectangles, with
//! all overlapping areas removed.
//!
//! offsetx and offsety are added to all rectangles' returned coordinates.

rectarea add(const rectarea &,
	     const rectarea &,
	     coord_t offsetx=0,
	     coord_t offsety=0);

//! Return an intersection set of two rectangle sets.

//! The returned rectangle set consists of only rectangular areas present
//! in both sets.
//!
//! offsetx and offsety are added to all rectangles' returned coordinates.
//!
//! offsetx and offsety are added to all rectangles' returned coordinates.

rectarea intersect(const rectarea &,
		   const rectarea &,
		   coord_t offsetx=0,
		   coord_t offsety=0);

//! Return an intersection set of two rectangle sets.

//! With the second set consisting of one widget. Most often used optimized
//! path.
rectarea intersect(const rectarea &,
			const rectangle &,
			coord_t offsetx=0,
			coord_t offsety=0);

//! Subtract two rectangle sets.

//! The returned rectangle set consists of the first rectangle set, with
//! rectangles in the second set removed.
//!
//! offsetx and offsety are added to all rectangles' returned coordinates.

rectarea subtract(const rectarea &,
		  const rectarea &,
		  coord_t ofssetx=0,
		  coord_t offsety=0);

//! Return a bounding rectangle for the given rectangle set

rectangle bounds(const rectarea &);

LIBCXXW_NAMESPACE_END

#endif
