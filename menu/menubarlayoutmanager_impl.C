/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menubarlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

menubarlayoutmanagerObj::implObj::implObj(const ref<containerObj::implObj>
					  &container_impl)
	: gridlayoutmanagerObj::implObj(container_impl)
{
}

menubarlayoutmanagerObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
