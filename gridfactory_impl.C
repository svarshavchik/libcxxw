/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gridfactory.H"
#include "gridlayoutmanager.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

gridfactoryObj::implObj::implObj(dim_t row, dim_t col,
				 const ref<grid_map_infoObj> &grid_map)
	: new_grid_element{row, col, grid_map}
{
}

gridfactoryObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
