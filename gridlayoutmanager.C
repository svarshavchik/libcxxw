/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridfactory.H"
#include "x/w/element.H"
#include "gridfactory.H"
#include "metrics_grid_pos.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), lock(impl->grid_map), impl(impl)
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

gridfactory gridlayoutmanagerObj::create()
{
	auto me=gridlayoutmanager(this);

	return gridfactory::create(me,
				   ref<gridfactoryObj::implObj>::create(me));
}

void gridlayoutmanagerObj::erase(dim_t x, dim_t y)
{
}

elementptr gridlayoutmanagerObj::get(dim_t x, dim_t y)
{
	return elementptr();
}

LIBCXXW_NAMESPACE_END
