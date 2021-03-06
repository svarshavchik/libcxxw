/*
** Copyright 2017-2021 Double Precision, Inc.
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

	const focusable_impl impl;

	//! Constructor
	ownerObj(const focusable_impl &impl);

	//! Destructor
	~ownerObj();

	//! Implement get_impl().
	focusable_impl get_impl() const override;

	using focusableObj::get_impl;
};

LIBCXXW_NAMESPACE_END

#endif
