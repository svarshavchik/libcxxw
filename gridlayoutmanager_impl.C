/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: layoutmanagerObj::implObj(container_impl)
{
}

gridlayoutmanagerObj::implObj::~implObj()=default;

void gridlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
}

void gridlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const child_element &e)> &callback)
{
}

layoutmanager gridlayoutmanagerObj::implObj::create_public_object()
{
	return gridlayoutmanager::create(ref<implObj>(this));
}

LIBCXXW_NAMESPACE_END
