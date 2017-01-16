/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), impl(impl)
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

LIBCXXW_NAMESPACE_END
