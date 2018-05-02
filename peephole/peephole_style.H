/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_style_H
#define peephole_style_H

#include "x/w/alignment.H"

LIBCXXW_NAMESPACE_START

//! How a peephole scrolls elements that gain focus into view.

enum class LIBCXX_HIDDEN peephole_scroll {
	minimal, //!< Scroll the peephole minimally.
	centered, //!< Try to keep the input focus centered in the peephole
};

//! Peephole sizing algorithm.

enum class LIBCXX_HIDDEN peephole_algorithm {

	//! Default sizing algorithm

	//! We take the width/height of the peepholed element, the
	//! width/height of the peephole, and work with it.

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

struct LIBCXX_HIDDEN peephole_style {

	//! Alignment of the element when it is smaller than the peephole.
	halign horizontal_alignment=halign::left;

	//! Alignment of the element when it is smaller than the peephole.
	valign vertical_alignment=valign::top;

	//! How the peephole's width gets processed.

	peephole_algorithm width_algorithm=peephole_algorithm::automatic;

	//! How the peephole's height gets processed.

	peephole_algorithm height_algorithm=peephole_algorithm::automatic;

	//! Peephole scrolling strategy

	peephole_scroll scroll=peephole_scroll::minimal;
};

LIBCXXW_NAMESPACE_END

#endif
