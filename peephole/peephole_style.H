/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_style_H
#define peephole_style_H

#include <x/w/alignment.H>

LIBCXXW_NAMESPACE_START

//! Peephole style
struct LIBCXX_HIDDEN peephole_style {

 public:

	//! Horizontal alignment

	//! halign::fill expands the width of the peepholed element to be at
	//! least as wide as the peephole. This is used for combo-box
	//! dropdown lists, to pad them so that the popup is the same width
	//! as the main combobox display element.
	//!
	//! Other values are ignored.

	halign h_alignment=halign::left;
};

LIBCXXW_NAMESPACE_END

#endif
