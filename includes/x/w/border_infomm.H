/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_border_infomm_h
#define x_w_border_infomm_h

#include <x/w/picture.H>
#include <x/w/border_infommfwd.H>
#include <vector>

LIBCXXW_NAMESPACE_START

//! The type of a border

//! Specifies all properties of a border.
//!

struct border_infomm {

	//! Constructor
	border_infomm();

	//! Destructor
	~border_infomm();

	//! Border's colors

	//! Typically 1 color, but maybe two.

	std::vector<const_picture> colors;

	//! Configured width in millimeters
	double width=0;

	//! Configured height in millimeters
	double height=0;

	//! Rounded cornder
	bool rounded=false;

	//! Non-0, inner rounded radius, in millimeters

	double radius=0;

	//! If not empty, dashed border, this gives the dash lengths in mms.
	std::vector<double> dashes;

	//! Equality operator

	bool operator==(const border_infomm &o) const;
};

LIBCXXW_NAMESPACE_END

#endif
