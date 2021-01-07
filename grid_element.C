/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "grid_element.H"
#include "x/w/impl/current_border_impl.H"
#include "metrics_grid_pos.H"
#include "x/w/impl/element.H"
#include "x/w/impl/themedim_element.H"
#include "screen.H"
#include "defaulttheme.H"
#include "grid_map_info.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

new_grid_element_info
::new_grid_element_info(dim_t row,
			dim_t col,
			const container_impl &parent_container,
			const ref<grid_map_infoObj> &grid_map)
	: left_padding_set{grid_map->grid_horiz_padding},
	  right_padding_set{grid_map->grid_horiz_padding},
	  top_padding_set{grid_map->grid_vert_padding},
	  bottom_padding_set{grid_map->grid_vert_padding},
	  row{row},
	  col{col},
	  screen_impl{parent_container->container_element_impl()
		      .get_screen()->impl}
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
	: existing_grid_element_info{info},
	  themedim_element<left_padding_tag>{info.left_padding_set,
			  info.screen_impl,
			  themedimaxis::width},
	  themedim_element<right_padding_tag>{info.right_padding_set,
			  info.screen_impl,
			  themedimaxis::width},
	  themedim_element<top_padding_tag>{info.top_padding_set,
			  info.screen_impl,
			  themedimaxis::width},
	  themedim_element<bottom_padding_tag>{info.bottom_padding_set,
			  info.screen_impl,
			  themedimaxis::width},
	  grid_element{grid_element},
	  pos{metrics::grid_pos::create()},
	  initialized_thread_only{false}
{
}

grid_elementObj::~grid_elementObj()=default;

void grid_elementObj::initialize(ONLY IN_THREAD,
				 const ref<screenObj::implObj> &screen_impl)
{
	initialized(IN_THREAD)=true;
	themedim_element<left_padding_tag>::initialize(IN_THREAD, screen_impl);
	themedim_element<right_padding_tag>::initialize(IN_THREAD, screen_impl);
	themedim_element<top_padding_tag>::initialize(IN_THREAD, screen_impl);
	themedim_element<bottom_padding_tag>::initialize(IN_THREAD,
							 screen_impl);
	recalculate_padding(IN_THREAD);
}

void grid_elementObj::theme_updated(ONLY IN_THREAD,
				    const const_defaulttheme &new_theme)
{
	themedim_element<left_padding_tag>::theme_updated(IN_THREAD, new_theme);
	themedim_element<right_padding_tag>::theme_updated(IN_THREAD,
							   new_theme);
	themedim_element<top_padding_tag>::theme_updated(IN_THREAD, new_theme);
	themedim_element<bottom_padding_tag>::theme_updated(IN_THREAD,
							    new_theme);
	recalculate_padding(IN_THREAD);
}

void grid_elementObj::recalculate_padding(ONLY IN_THREAD)
{
	left_padding=themedim_element<left_padding_tag>::pixels(IN_THREAD);
	dim_t total=
		dim_t::truncate(left_padding+
				themedim_element<right_padding_tag>
				::pixels(IN_THREAD));

	if (total == dim_t::infinite())
		--total;

	total_horiz_padding=total;

	top_padding=themedim_element<top_padding_tag>::pixels(IN_THREAD);

	total=dim_t::truncate(top_padding+
			      themedim_element<bottom_padding_tag>
			      ::pixels(IN_THREAD));

	if (total == dim_t::infinite())
		--total;
	total_vert_padding=total;
}

bool grid_elementObj::takes_up_space(ONLY IN_THREAD) const
{
	// Recalculate gets triggered in requested_child_visibility_changed()
	// hook, so when requested_visibility changes this gets recalculated.

	return grid_element->impl->data(IN_THREAD).requested_visibility ||
		!remove_when_hidden;
}

LIBCXXW_NAMESPACE_END
