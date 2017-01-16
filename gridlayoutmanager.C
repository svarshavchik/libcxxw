/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "x/w/element.H"
#include "gridfactory.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), impl(impl)
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

factory gridlayoutmanagerObj::insert(dim_t x, dim_t y,
				     dim_t width,
				     dim_t height)
{
	return gridfactory::create(gridlayoutmanager(this),
				   x, y, width, height);
}

void gridlayoutmanagerObj::erase(dim_t x, dim_t y)
{
}

elementptr gridlayoutmanagerObj::get(dim_t x, dim_t y)
{
	return elementptr();
}

LIBCXXW_NAMESPACE_END
