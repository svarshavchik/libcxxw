/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_peephole_style_H
#define x_w_peephole_style_H

#include <x/w/alignment.H>
#include <x/w/dim_axis_arg.H>
#include <variant>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! How a peephole scrolls elements that gain focus into view.

//! \see peephole_style

enum class peephole_scroll {
	minimal, //!< Scroll the peephole minimally.
	centered, //!< Try to keep the input focus centered in the peephole
};

//! Peephole sizing algorithm.

//! \see peephole_style

enum class peephole_algorithm {

	//! Default sizing algorithm

	//! We take the width/height of the peepholed element, the
	//! width/height of the peephole, and work with it.
        //!
        //! This is used mostly internally by the library in situation where
        //! the peephole's container completely controls the peephole size.
        //! For example, the peephole is in a container that uses the
        //! grid layout manager, with the peephole's width or height
        //! alignment set to 'fill'.

	automatic,

	//! Stretch the peephole to match the peepholed element's dimension.

	//! Always set the width or the height of the peephole to match
	//! the width or height of the peepholed element. This effectively
	//! turns this peephole into a single-dimension peephole. If the
	//! width gets stretched, the peephole operates only in the vertical
	//! dimension, and vice versa
	stretch_peephole
};

//! Peephole style.

struct peephole_style {

	//! How the peephole's width gets set.

	//! The peephole's width is either explicitly specified (as a
	//! dim_axis_arg), or as a peephole_algorithm.
	std::variant<peephole_algorithm, dim_axis_arg
		     > width_algorithm=peephole_algorithm::stretch_peephole;

	//! How the peephole's height gets set.

	//! The peephole's height is either explicitly specified (as a
	//! dim_axis_arg), or as a peephole_algorithm.
	//!
	//! The library also uses an internal option that specifies the
	//! height as a minimum or maximum number of rows. This is used
	//! internally by the \ref new_listlayoutmanager "list layout manager".

	std::variant<peephole_algorithm, std::tuple<size_t, size_t>,
		     dim_axis_arg
		     > height_algorithm=peephole_algorithm::stretch_peephole;

	//! Alignment of the element when it is smaller than the peephole.
	halign horizontal_alignment=halign::left;

	//! Alignment of the element when it is smaller than the peephole.
	valign vertical_alignment=valign::top;

	//! Peephole scrolling strategy

	peephole_scroll scroll=peephole_scroll::minimal;

	//! Truncate the peepholed element's width to peephole width.

	//! Even if the peephole width is smaller than the peepholed element's
	//! minimum width, the peepholed element's width never exceeds the
	//! peephole's width.
	bool width_truncate=false;

	//! Truncate the peepholed element's height to peephole height.

	//! Even if the peephole height is smaller than the peepholed element's
	//! minimum height, the peepholed element's height never exceeds the
	//! peephole's height.
	bool height_truncate=false;
};

LIBCXXW_NAMESPACE_END

#endif
