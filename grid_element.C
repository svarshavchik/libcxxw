/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "grid_element.H"
#include "current_border_impl.H"
#include "metrics_grid_pos.H"
#include "x/w/element.H"

LIBCXXW_NAMESPACE_START

new_grid_element_info
::new_grid_element_info(dim_t row,
			dim_t col,
			const current_border_impl &initial_border)
	: row(row),
	  col(col),
	  left_border(initial_border),
	  right_border(initial_border),
	  top_border(initial_border),
	  bottom_border(initial_border)
{
}

new_grid_element_info
::new_grid_element_info(const new_grid_element_info &)=default;

new_grid_element_info::~new_grid_element_info()=default;

grid_elementObj::grid_elementObj(const new_grid_element_info &info,
				 const element &grid_element)
	: new_grid_element_info(info),
	grid_element(grid_element),
	pos(metrics::grid_pos::create())
{
}

grid_elementObj::~grid_elementObj()=default;

LIBCXXW_NAMESPACE_END
