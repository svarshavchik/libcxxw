/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer.H"
#include "listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::listcontainerObj(const focusable_impl &list_focusable_impl,
				   const container_impl &container_impl,
				   const layout_impl &list_layout_impl)
	: focusable_containerObj{container_impl, list_layout_impl},
	  list_focusable_impl{list_focusable_impl}
{
}

listcontainerObj::~listcontainerObj()=default;

focusable_impl listcontainerObj::get_impl() const
{
	return list_focusable_impl;
}

LIBCXXW_NAMESPACE_END
