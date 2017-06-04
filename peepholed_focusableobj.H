/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_focusableobj_h
#define peepholed_focusableobj_h

#include "peepholed_focusablefwd.H"
#include "scrollbar/scrollbarfwd.H"
#include "focus/focusablefwd.H"
#include "x/w/focusablefwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Define a peepholed focusable

//! \see peepholed_focusable

class LIBCXX_HIDDEN peepholed_focusableObj : virtual public obj {

 public:

	//! The focusable element in the peephole
	const focusable peepholed_element;

	//! The peephole's vertical scrollbar
	const scrollbar vertical_scrollbar;

	//! The peephole's horizontal scrollbar

	const scrollbar horizontal_scrollbar;

	//! Constructor
	peepholed_focusableObj(const focusable &peepholed_element,
			       const scrollbar &vertical_scrollbar,
			       const scrollbar &horizontal_scrollbar);

	//! Destructor
	~peepholed_focusableObj();

	//! Implement the display element's method inherited from focusableObj.

	focusable_impl get_impl() const;

	//! Implement the display element's method inherited from focusableObj.

	size_t internal_impl_count() const;

	//! Implement the display element's method inherited from focusableObj.

	focusable_impl get_impl(size_t n) const;
};

LIBCXXW_NAMESPACE_END

#endif
