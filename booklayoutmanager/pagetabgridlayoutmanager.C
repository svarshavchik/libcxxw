/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetabgridlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

pagetabgridlayoutmanagerObj
::pagetabgridlayoutmanagerObj(const ref<implObj> &impl)
	: gridlayoutmanagerObj{impl}, impl{impl}
{
}

pagetabgridlayoutmanagerObj::~pagetabgridlayoutmanagerObj()=default;

LIBCXXW_NAMESPACE_END
