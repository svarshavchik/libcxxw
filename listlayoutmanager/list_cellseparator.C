/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_cellseparator.H"

LIBCXXW_NAMESPACE_START

list_cellseparatorObj::list_cellseparatorObj()
	: list_cellObj{valign::middle}
{
}

std::pair<metrics::axis, metrics::axis>
list_cellseparatorObj::cell_get_metrics(ONLY IN_THREAD, dim_t preferred_width)
{
	return {{},{}};
}


void list_cellseparatorObj::cell_redraw(ONLY IN_THREAD,
					element_drawObj &draw,
					const draw_info &di,
					clip_region_set &clipped,
					bool draw_as_disabled,
					richtext_draw_boundaries &boundaries)
{
}

void list_cellseparatorObj::cell_initialize(ONLY IN_THREAD,
					   const defaulttheme &initial_theme)
{
}

void list_cellseparatorObj::cell_theme_updated(ONLY IN_THREAD,
					      const defaulttheme &initial_theme)
{
}

bool list_cellseparatorObj::cell_is_separator()
{
	return true;
}

LIBCXXW_NAMESPACE_END
