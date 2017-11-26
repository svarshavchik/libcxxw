/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_style_H
#define peephole_style_H

#include <x/w/alignment.H>

LIBCXXW_NAMESPACE_START

//! How a peephole scrolls elements that gain focus into view.

enum class LIBCXX_HIDDEN peephole_scroll {
	minimal, //!< Scroll the peephole minimally.
	centered, //!< Try to keep the input focus centered in the peephole
};

//! Peephole style.

struct LIBCXX_HIDDEN peephole_style {

	//! Alignment of the element when it is smaller than the peephole.
	halign horizontal_alignment=halign::left;

	//! Alignment of the element when it is smaller than the peephole.
	valign vertical_alignment=valign::top;

	//! Peephole scrolling strategy

	peephole_scroll scroll=peephole_scroll::minimal;
};

LIBCXXW_NAMESPACE_END

#endif
