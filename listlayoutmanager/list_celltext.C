/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_celltext.H"
#include "richtext/richtext_draw_info.H"
#include "richtext/richtext_draw_boundaries.H"

LIBCXXW_NAMESPACE_START


list_celltextObj::list_celltextObj(const richtextstring &string,
				   halign alignment,
				   dim_t word_wrap_width)
	: richtextObj(string, alignment, word_wrap_width)
{
}

std::pair<metrics::axis, metrics::axis>
list_celltextObj::cell_get_metrics(IN_THREAD_ONLY, dim_t preferred_width,
				   bool visible)
{
	return get_metrics(IN_THREAD, preferred_width, visible);
}


void list_celltextObj::cell_redraw(IN_THREAD_ONLY,
				   element_drawObj &draw,
				   const draw_info &di,
				   bool draw_as_disabled,
				   const richtext_draw_boundaries &boundaries)
{
	richtext_draw_info rdi;

	rdi.draw_as_disabled=draw_as_disabled;
	text_width(boundaries.draw_bounds.width);

	full_redraw(IN_THREAD, draw, rdi, di, boundaries);
}

void list_celltextObj::cell_initialize(IN_THREAD_ONLY,
				       const defaulttheme &initial_theme)
{
	theme_updated(IN_THREAD, initial_theme);
}

void list_celltextObj::cell_theme_updated(IN_THREAD_ONLY,
					  const defaulttheme &initial_theme)
{
	theme_updated(IN_THREAD, initial_theme);
}

bool list_celltextObj::cell_is_separator()
{
	return false;
}

LIBCXXW_NAMESPACE_END
