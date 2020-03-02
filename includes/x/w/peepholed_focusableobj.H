/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholed_focusableobj_h
#define x_w_peepholed_focusableobj_h

#include <x/w/focusablefwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC peepholed_focusableObj;

//! A focusable in a peephole.

//! An internal class that's used to derive a focusable widget inside
//! a scrollable peephole.
//!
//! \internal

class peepholed_focusableObj : public focusable_containerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation

	const ref<implObj> impl;

	//! Constructor
	peepholed_focusableObj(const ref<implObj> &impl,
			       const layout_impl &container_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~peepholed_focusableObj() LIBCXX_HIDDEN;

	//! Inherited from focusableObj
	focusable_impl get_impl() const override LIBCXX_HIDDEN;

	//! Inherited from focusableObj
	void do_get_impl(const function<internal_focusable_cb> &)
		const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
