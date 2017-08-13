/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

listitemcontainerObj
::listitemcontainerObj(const ref<implObj> &impl,
		       const ref<listitemlayoutmanagerObj::implObj> &l)
	: containerObj(impl, l),
	  impl(impl)
{
}

listitemcontainerObj::~listitemcontainerObj()=default;

LIBCXXW_NAMESPACE_END
