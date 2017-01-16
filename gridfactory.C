/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gridfactory.H"
#include "x/w/factory.H"
#include "gridlayoutmanager.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

gridfactoryObj::gridfactoryObj(const gridlayoutmanager &gridlayout,
			       dim_t x, dim_t y,
			       dim_t width, dim_t height)
	: factoryObj(gridlayout->impl->container_impl),
	  gridlayout(gridlayout),
	  x(x),
	  y(y),
	  width(width),
	  height(height)
{
}

gridfactoryObj::~gridfactoryObj()=default;

void gridfactoryObj::created(const child_element &new_element)
{
}

LIBCXXW_NAMESPACE_END
