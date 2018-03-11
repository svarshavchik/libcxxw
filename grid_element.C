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
#include "grid_map_info.H"

LIBCXXW_NAMESPACE_START

new_grid_element_info
::new_grid_element_info(dim_t row,
			dim_t col,
			const ref<grid_map_infoObj> &grid_map)
	: row(row),
	  col(col),
	  left_padding_set("grid_horiz_padding"),
	  right_padding_set(left_padding_set),
	  top_padding_set("grid_vert_padding"),
	  bottom_padding_set(top_padding_set)
{
	auto row_default=grid_map->row_defaults
		.find(metrics::grid_xy::truncate(row));

	if (row_default != grid_map->row_defaults.end())
	{
		vertical_alignment=row_default->second.vertical_alignment;
		top_padding_set=row_default->second.top_padding_set;
		bottom_padding_set=row_default->second.bottom_padding_set;
	}

	auto col_default=grid_map->column_defaults
		.find(metrics::grid_xy::truncate(col));

	if (col_default != grid_map->column_defaults.end())
	{
		horizontal_alignment=col_default->second.horizontal_alignment;
		left_padding_set=col_default->second.left_padding_set;
		right_padding_set=col_default->second.right_padding_set;
	}
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
	left_padding=theme->get_theme_width_dim_t(left_padding_set);
	dim_t total=
		dim_t::truncate(left_padding+
				theme->get_theme_width_dim_t(right_padding_set))
		;

	if (total == dim_t::infinite())
		--total;

	total_horiz_padding=total;

	top_padding=theme->get_theme_height_dim_t(top_padding_set);

	total=dim_t::truncate(top_padding+
			      theme->get_theme_height_dim_t(bottom_padding_set))
		;

	if (total == dim_t::infinite())
		--total;
	total_vert_padding=total;
}

bool grid_elementObj::takes_up_space(IN_THREAD_ONLY) const
{
	// Recalculate gets triggered in requested_child_visibility_changed()
	// hook, so when requested_visibility changes this gets recalculated.

	return grid_element->impl->data(IN_THREAD).requested_visibility ||
		!remove_when_hidden;
}

LIBCXXW_NAMESPACE_END
