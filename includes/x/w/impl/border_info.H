/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_border_info_h
#define x_w_impl_border_info_h

#include <x/w/picture.H>
#include <x/w/types.H>
#include <x/w/impl/border_infofwd.H>
#include <x/w/impl/background_colorfwd.H>
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

struct border_info {

	//! Constructor
	border_info(const background_color &color1);

	//! Destructor
	~border_info();

	//! Border color.

	background_color color1;

	//! Optional 2nd color, for dashed broders.
	background_colorptr color2;

	//! Configured width in pixels
	dim_t width=1;

	//! Configured height in pixels
	dim_t height=1;

	//! Rounded corner, and/or radius

	//! 0: square corner

	//! 1: rounded corner

	//! >1: rounded corner, with inside radius
	dim_t hradius=0;

	//! Rounded corner, and/or radius

	//! Both hradius and vradius=0: square corner.

	//! Both hradius and vradius are 2 or more: rounded corner, with
	//! inside radius.

	//! Otherwise rounded outside corner.

	dim_t vradius=0;

	//! If not empty, dashed border, this gives the dash lengths in pixels.
	std::vector<uint8_t> dashes;

	//! Equality operator

	bool operator==(const border_info &o) const;

	//! Ordering operator

	bool compare(const border_info &o) const;

	//! This border's width or height is 0, or it has no colors
	bool no_border() const;
};

LIBCXXW_NAMESPACE_END

#endif
