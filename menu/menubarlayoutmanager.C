/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/menubarlayoutmanager.H"
#include "menu/menubarlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

menubarlayoutmanagerObj::menubarlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), impl(impl)
{
}

menubarlayoutmanagerObj::~menubarlayoutmanagerObj()=default;

LIBCXXW_NAMESPACE_END
