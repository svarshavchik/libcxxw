/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

listitemlayoutmanagerObj::listitemlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl),
	  impl(impl)
{
}

listitemlayoutmanagerObj::~listitemlayoutmanagerObj()=default;


LIBCXXW_NAMESPACE_END
