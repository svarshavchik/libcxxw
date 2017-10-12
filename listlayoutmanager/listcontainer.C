/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer.H"
#include "listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::listcontainerObj(const ref<focusableImplObj> &focusable_impl,
				   const ref<containerObj::implObj> &container_impl,
				   const ref<layoutmanagerObj::implObj>
				   &list_impl)
	: focusable_containerObj(container_impl, list_impl),
	  focusable_impl(focusable_impl)
{
}

listcontainerObj::~listcontainerObj()=default;

ref<focusableImplObj> listcontainerObj::get_impl() const
{
	return focusable_impl;
}

LIBCXXW_NAMESPACE_END
