/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::listcontainerObj(const ref<implObj> &impl,
				   const ref<listlayoutmanagerObj::implObj>
				   &list_impl)
	: focusable_containerObj(impl, list_impl),
	  impl(impl)
{
}

listcontainerObj::~listcontainerObj()=default;

ref<focusableImplObj> listcontainerObj::get_impl() const
{
	return impl;
}

LIBCXXW_NAMESPACE_END
