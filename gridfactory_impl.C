/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gridfactory.H"
#include "gridlayoutmanager.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

gridfactoryObj::implObj::implObj(const gridlayoutmanager &gridlayout,
				 dim_t row, dim_t col)
	: new_grid_element{row, col,
		gridlayout->impl->get_custom_border(border_infomm())}
{
}

gridfactoryObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END