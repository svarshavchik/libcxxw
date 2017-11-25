/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_focusable_impl_h
#define peepholed_focusable_impl_h

#include "focus/focusablefwd.H"
#include "x/w/peepholed_focusableobj.H"
#include "x/w/focusablefwd.H"
#include "x/w/scrollbarfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Define a peepholed focusable

//! \see peepholed_focusable

class LIBCXX_HIDDEN peepholed_focusableObj::implObj : virtual public obj {

 public:

	//! The focusable element in the peephole
	const focusable peepholed_element;

	//! The peephole's vertical scrollbar
	const scrollbar vertical_scrollbar;

	//! The peephole's horizontal scrollbar

	const scrollbar horizontal_scrollbar;

	//! Constructor
	implObj(const focusable &peepholed_element,
			       const scrollbar &vertical_scrollbar,
			       const scrollbar &horizontal_scrollbar);

	//! Destructor
	~implObj();

	//! Implement the display element's method inherited from focusableObj.

	focusable_impl get_impl() const;

	//! Implement the display element's method inherited from focusableObj.

	void do_get_impl(const function<internal_focusable_cb> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
