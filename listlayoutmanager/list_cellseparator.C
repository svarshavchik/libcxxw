/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_cellseparator.H"

LIBCXXW_NAMESPACE_START


list_cellseparatorObj::list_cellseparatorObj()=default;

std::pair<metrics::axis, metrics::axis>
list_cellseparatorObj::cell_get_metrics(IN_THREAD_ONLY, dim_t preferred_width,
					bool visible)
{
	return {{},{}};
}


void list_cellseparatorObj::cell_redraw(IN_THREAD_ONLY,
				       element_drawObj &draw,
				       const draw_info &di,
				       bool draw_as_disabled,
				       const richtext_draw_boundaries
				       &boundaries)
{
}

void list_cellseparatorObj::cell_initialize(IN_THREAD_ONLY,
					   const defaulttheme &initial_theme)
{
}

void list_cellseparatorObj::cell_theme_updated(IN_THREAD_ONLY,
					      const defaulttheme &initial_theme)
{
}

bool list_cellseparatorObj::cell_is_separator()
{
	return true;
}

LIBCXXW_NAMESPACE_END