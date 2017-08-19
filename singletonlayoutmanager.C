/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "singletonlayoutmanager.H"
#include "singletonlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

singletonlayoutmanagerObj::singletonlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), impl(impl)
{
}

singletonlayoutmanagerObj::~singletonlayoutmanagerObj()=default;

LIBCXXW_NAMESPACE_END
