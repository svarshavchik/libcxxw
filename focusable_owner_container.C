/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focusable_owner_container.H"
#include "x/w/impl/focus/focusable.H"

LIBCXXW_NAMESPACE_START

focusable_owner_containerObj
::focusable_owner_containerObj(const ref<containerObj::implObj> &impl,
				     const ref<layoutmanagerObj::implObj>
				     &layout_impl,
				     const focusable_impl &f_impl)
	: containerObj(impl, layout_impl),
	  focusableObj::ownerObj(f_impl)
{
}

focusable_owner_containerObj::~focusable_owner_containerObj()=default;

focusable_impl focusable_owner_containerObj::get_impl() const
{
	return focusableObj::ownerObj::get_impl();
}

LIBCXXW_NAMESPACE_END
