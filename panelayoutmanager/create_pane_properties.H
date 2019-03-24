/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef create_pane_properties_h
#define create_pane_properties_h

#include "x/w/dim_arg.H"
#include "x/w/rgb.H"
#include "x/w/alignment.H"
#include "x/w/scrollbar.H"
#include "x/w/scrollbar_appearancefwd.H"

LIBCXXW_NAMESPACE_START

//! Properties of a new pane.

struct LIBCXX_HIDDEN create_pane_properties_t {

	//! Requested size of the next pane

	dim_arg dimension;

	//! Requested background color for the next pane.

	std::optional<color_arg> background_color;

	//! Padding
	dim_arg left_padding_set;

	//! Padding
	dim_arg right_padding_set;

	//! Padding
	dim_arg top_padding_set;

	//! Padding
	dim_arg bottom_padding_set;

	//! Custom alignment
	halign horizontal_alignment=halign::left;

	//! Custom alignment
	valign vertical_alignment=valign::top;

	//! Scrollbar visibility
	scrollbar_visibility pane_scrollbar_visibility=
		scrollbar_visibility::automatic_reserved;

	//! Scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;

	//! Constructor
	create_pane_properties_t();
};


LIBCXXW_NAMESPACE_END

#endif
