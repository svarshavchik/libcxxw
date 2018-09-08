/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "tablelayoutmanager/tablelayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

tablelayoutmanagerObj::tablelayoutmanagerObj(const ref<implObj> &impl)
	: listlayoutmanagerObj{impl},
	  impl{impl}
{
}

tablelayoutmanagerObj::~tablelayoutmanagerObj()=default;

LIBCXXW_NAMESPACE_END
