/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef border_implobj_h
#define border_implobj_h

#include "x/w/types.H"
#include "x/w/picture.H"
#include "border_implfwd.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! The type of a border

//! Specifies all properties of a border.
//!
//! border_implObj inherits from this. This makes it easy to clone() a border.
//!
//! \c width and \c height is the border size that's specified in the
//! theme. This is not the actual size of the border, which also depends on
//! whether the border is rounded, or not.
//!
//! calculate() computes the calculated values for the border size.

struct LIBCXX_HIDDEN border_info {

	//! Border's colors

	//! Typically 1 color, but maybe two.

	std::vector<const_picture> colors;

	//! Configured width in pixels
	dim_t width=1;

	//! Configured height in pixels
	dim_t height=1;

	//! Rounded corner, and/or radius

	//! 0: square corner

	//! 1: rounded corner

	//! >1: rounded corner, with inside radius
	dim_t radius=0;

	//! If not empty, dashed border, this gives the dash lengths
	std::vector<uint8_t> dashes;

	//! Equality operator

	bool operator==(const border_info &o) const
	{
		return width == o.width && height == o.height &&
			radius == o.radius && dashes == o.dashes &&
			colors == o.colors;
	}

	//! Ordering operator

	bool compare(const border_info &o) const
	{
		if (width + height < o.width + o.height)
			return true;
		if (o.width + o.height < width + height)
			return false;

		if (radius < o.radius)
			return true;
		if (o.radius < radius)
			return false;

		if (dashes < o.dashes)
			return true;

		if (o.dashes < o.dashes)
			return false;

		return colors < o.colors;
	}
};

//! A border implementation

//! \see border_impl

class LIBCXX_HIDDEN border_implObj: virtual public obj, public border_info {

 public:

	//! Default constructor
	border_implObj();

	//! Clone constructor
	border_implObj(const border_info &b);

	//! Destructor
	~border_implObj();

	//! Clone this border.
	border_impl clone() const;

	//! Compute the calculated values from border_info.

	void calculate();

	//! Sum of all dashes, precalculated for speed
	dim_t calculated_dashes_sum;

	//! Actual border width
	dim_t calculated_border_width;

	//! Actual border height
	dim_t calculated_border_height;

	//! A border_radius value of 2 or more has an inner radius.
	static constexpr dim_t inner_radius(dim_t r)
	{
		return r < 2 ? 0:r;
	}

	//! A border_radius value of 2 or more has an inner radius.

	dim_t inner_radius() const
	{
		return inner_radius(radius);
	}
};

LIBCXXW_NAMESPACE_END

#endif
