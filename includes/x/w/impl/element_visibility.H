/*
** Copyright 2023 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_element_visibility_h
#define x_w_impl_element_visibility_h

#include <x/w/rectangle.H>
#include <variant>

LIBCXXW_NAMESPACE_START

//! What we want for ensure_visibility() to accomplish

struct element_visibility_t {

	//! Make sure that this part of me is visible.

	//! If this ends up being in a peephole the peephole gets scrolled to
	//! make this area comes into view.
	const rectangle r;

	//! We wanted to make sure that the entire widget is visible

	//! When a widget gets input focus it requested visibility for itself,
	//! in its entirety. If a focusable element is inside a peephole, and
	//! the focusable element request its entirety to be visible the
	//! peephole layout manager checks for this in order to avoid a
	//! spurious scroll to the top-right corner.

	const bool entire_visibility;
};


LIBCXXW_NAMESPACE_END

#endif
