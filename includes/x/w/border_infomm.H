/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_border_infomm_h
#define x_w_border_infomm_h

#include <x/w/rgb.H>
#include <x/w/border_infommfwd.H>
#include <x/w/dim_arg.H>
#include <vector>
#include <optional>

LIBCXXW_NAMESPACE_START

//! A border of an element in a grid.

//! The \ref gridlayoutmanager "grid layout manager" draws borders around
//! each grid element. This class specifies a custom grid element border.
//!
//! Borders of adjacent elements get collapsed together, with the "better"
//! border ending up on top, and ruling the roost.
//!
//! Other display elements also use borders, for various purposes.

struct border_infomm {

	//! Constructor

	//! Creates an empty border. No border. Zilch. There won't be a border.
	//! At the minimum, a border must have at least one color, and
	//! non-zero width and height. Otherwise this border isn't going to
	//! go anywhere.

	border_infomm();

	//! Destructor
	~border_infomm();

	//! Border color
	color_arg color1=transparent;

	//! Optional 2nd color, for dashed borders.
	std::optional<color_arg> color2;

	//! Border width.
	dim_arg width=0;

	//! Border height.
	dim_arg height=0;

	//! After converting width to pixels, multiply it by:
	unsigned width_scale=1;

	//! After converting height to pixels, multiply it by:
	unsigned height_scale=1;

	//! Round corners.
	bool rounded=false;

	//! If non-0, inner rounded radius.

	dim_arg hradius=0;

	//! After converting radius to pixels, multiply it by:
	unsigned hradius_scale=1;

	//! If non-0, inner rounded radius, in millimeters

	dim_arg vradius=0;

	//! After converting radius to pixels, multiply it by:
	unsigned vradius_scale=1;

	//! If not empty, dashed border, this gives the dash lengths in mms.
	std::vector<double> dashes;

	//! Equality operator

	bool operator==(const border_infomm &o) const;
};

LIBCXXW_NAMESPACE_END

#endif
