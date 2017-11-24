/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_border_infomm_h
#define x_w_border_infomm_h

#include <x/w/rgb.H>
#include <x/w/border_infommfwd.H>
#include <vector>

LIBCXXW_NAMESPACE_START

//! A border of an element in a grid.

//! The \ref gridlayoutmanager "grid layout manager" draws borders around
//! each grid element. This class specifies a custom grid element border.
//!
//! Borders of adjacent elements get collapsed together, with the "better"
//! border ending up on top, and ruling the roost.

struct border_infomm {

	//! Constructor

	//! Creates an empty border. No border. Zilch. There won't be a border.
	//! At the minimum, a border must have at least one color, and
	//! non-zero width and height. Otherwise this border isn't going to
	//! go anywhere.

	border_infomm();

	//! Destructor
	~border_infomm();

	//! Border's colors

	//! Typically 1 color, but maybe two.

	std::vector<color_arg> colors;

	//! Border width in millimeters
	double width=0;

	//! Border height in millimeters
	double height=0;

	//! Round corners.
	bool rounded=false;

	//! If non-0, inner rounded radius, in millimeters

	double radius=0;

	//! If not empty, dashed border, this gives the dash lengths in mms.
	std::vector<double> dashes;

	//! Equality operator

	bool operator==(const border_infomm &o) const;
};

LIBCXXW_NAMESPACE_END

#endif
