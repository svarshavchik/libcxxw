/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "grid_element.H"
#include "current_border_impl.H"
#include "metrics_grid_pos.H"
#include "element.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

new_grid_element_info
::new_grid_element_info(dim_t row,
			dim_t col)
	: row(row),
	  col(col)
{
}

new_grid_element_info
::new_grid_element_info(const new_grid_element_info &)=default;

new_grid_element_info::~new_grid_element_info()=default;


grid_element_padding_lock::grid_element_padding_lock(const screen &my_screen)
	: my_screen(my_screen),
	  theme_lock(my_screen->impl->current_theme)
{
}

grid_element_padding_lock::~grid_element_padding_lock()=default;

grid_elementObj::grid_elementObj(const new_grid_element_info &info,
				 const element &grid_element,
				 const grid_element_padding_lock &lock)
	: new_grid_element_info(info),
	grid_element(grid_element),
	pos(metrics::grid_pos::create())
{
	calculate_padding(lock);
}

grid_elementObj::~grid_elementObj()=default;

void grid_elementObj::calculate_padding(const grid_element_padding_lock &lock)
{
	left_padding=lock.my_screen->impl->compute_width(lock.theme_lock,
							 left_paddingmm);
	dim_t total=dim_t::truncate(left_padding+
				    lock.my_screen->impl
				    ->compute_width(lock.theme_lock,
						    right_paddingmm));

	if (total == dim_t::infinite())
		--total;

	total_horiz_padding=total;

	top_padding=lock.my_screen->impl->compute_height(lock.theme_lock,
							 top_paddingmm);

	total=dim_t::truncate(top_padding+
			      lock.my_screen->impl
			      ->compute_height(lock.theme_lock,
					       bottom_paddingmm));

	if (total == dim_t::infinite())
		--total;
	total_vert_padding=total;
}

bool grid_elementObj::takes_up_space(IN_THREAD_ONLY) const
{
	return grid_element->impl->data(IN_THREAD).inherited_visibility ||
		!remove_when_hidden;
}

LIBCXXW_NAMESPACE_END
