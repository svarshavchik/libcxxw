/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusableownerobj_h
#define x_w_focusableownerobj_h

#include <x/w/focusableobj.H>

LIBCXXW_NAMESPACE_START

//! Subclass of focusableObj which owns the internal implementation object.

//! \see focusable

class focusableObj::ownerObj : public focusableObj {

public:
	//! Internal implementation details.

	const ref<focusableObj::implObj> impl;

	//! Constructor
	ownerObj(const ref<focusableObj::implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~ownerObj() LIBCXX_HIDDEN;

	//! Implement get_impl().
	ref<focusableObj::implObj> get_impl() const override;

	using focusableObj::get_impl;
};

LIBCXXW_NAMESPACE_END

#endif
