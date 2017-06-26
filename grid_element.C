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
#include "defaulttheme.H"

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

grid_elementObj::grid_elementObj(const new_grid_element_info &info,
				 const element &grid_element)
	: new_grid_element_info(info),
	  grid_element(grid_element),
	  pos(metrics::grid_pos::create()),
	  initialized_thread_only(false)
{
}

grid_elementObj::~grid_elementObj()=default;

void grid_elementObj::initialize(IN_THREAD_ONLY)
{
	initialized(IN_THREAD)=true;

	auto theme=*current_theme_t::lock{
		grid_element->get_screen()->impl->current_theme
	};

	theme_updated(theme);
}

void grid_elementObj::theme_updated(const defaulttheme &theme)
{
	left_padding=theme->compute_width(left_paddingmm);
	dim_t total=dim_t::truncate(left_padding+
				    theme->compute_width(right_paddingmm));

	if (total == dim_t::infinite())
		--total;

	total_horiz_padding=total;

	top_padding=theme->compute_height(top_paddingmm);

	total=dim_t::truncate(top_padding+
			      theme->compute_height(bottom_paddingmm));

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
