/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_owner_containerobj_h
#define focusable_owner_containerobj_h

#include "focusable_owner_containerfwd.H"
#include "x/w/focusable_containerobj.H"
#include "x/w/focusableownerobj.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN focusable_owner_containerObj :
	public containerObj,
	public focusableObj::ownerObj {

 public:
	focusable_owner_containerObj(const container_impl &impl,
				     const layout_impl &container_layout_impl,
				     const focusable_impl &f_impl)
		LIBCXX_HIDDEN;
	~focusable_owner_containerObj();

	focusable_impl get_impl() const override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
