/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholed_focusableobj_h
#define x_w_peepholed_focusableobj_h

#include <x/w/focusablefwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A focusable in a peephole.

//! An internal class that's used to derive a focusable element inside
//! a scrollable peephole.
//!
//! \internal

class peepholed_focusableObj : public focusable_containerObj {

public:
	class implObj;

	//! Internal implementation

	const ref<implObj> impl;

	//! Constructor
	peepholed_focusableObj(const ref<implObj> &impl,
			       const ref<layoutmanagerObj::implObj>
			       &layout_impl) LIBCXX_HIDDEN;

	//! Destructor
	~peepholed_focusableObj() LIBCXX_HIDDEN;

	//! Inherited from focusableObj
	ref<focusableImplObj> get_impl() const override LIBCXX_HIDDEN;

private:

	//! Inherited from focusableObj

	//! \internal
	size_t internal_impl_count() const override LIBCXX_HIDDEN;

	//! Inherited from focusableObj

	//! \internal
	ref<focusableImplObj> get_impl(size_t) const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif