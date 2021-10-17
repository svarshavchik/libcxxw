/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "tablelayoutmanager/tablelayoutmanager_impl.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

tablelayoutmanagerObj::tablelayoutmanagerObj(const ref<implObj> &impl)
	: listlayoutmanagerObj{impl},
	  impl{impl}
{
}

tablelayoutmanagerObj::~tablelayoutmanagerObj()=default;

factory tablelayoutmanagerObj::replace_header(size_t column)
{
	return impl->header_layoutmanager_impl
		->create_gridlayoutmanager()
		->replace_cell(0, column);
}

element tablelayoutmanagerObj::header(size_t column) const
{
	notmodified();
	return impl->header_layoutmanager_impl->lock_and_get(0, column);
}

LIBCXXW_NAMESPACE_END
