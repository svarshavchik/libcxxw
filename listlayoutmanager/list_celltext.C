/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_celltext.H"
#include "richtext/richtext_draw_info.H"
#include "richtext/richtext_draw_boundaries.H"

LIBCXXW_NAMESPACE_START


list_celltextObj::list_celltextObj(const richtext_alteration_config
				   &richtext_alteration,
				   const richtextstring &string,
				   halign alignment,
				   dim_t word_wrap_width)
	: richtextObj{string, alignment, word_wrap_width},
	  richtext_alteration{richtext_alteration}
{
}

std::pair<metrics::axis, metrics::axis>
list_celltextObj::cell_get_metrics(ONLY IN_THREAD, dim_t preferred_width)
{
	return get_metrics(preferred_width);
}


void list_celltextObj::cell_redraw(ONLY IN_THREAD,
				   element_drawObj &draw,
				   const draw_info &di,
				   clip_region_set &clipped,
				   bool draw_as_disabled,
				   richtext_draw_boundaries &boundaries)
{
	richtext_draw_info rdi{richtext_alteration};

	rdi.draw_as_disabled=draw_as_disabled;
	text_width(boundaries.draw_bounds.width);

	full_redraw(IN_THREAD, draw, rdi, di, clipped, boundaries);
}

void list_celltextObj::cell_initialize(ONLY IN_THREAD,
				       const defaulttheme &initial_theme)
{
	theme_updated(IN_THREAD, initial_theme);
}

void list_celltextObj::cell_theme_updated(ONLY IN_THREAD,
					  const defaulttheme &initial_theme)
{
	theme_updated(IN_THREAD, initial_theme);
}

bool list_celltextObj::cell_is_separator()
{
	return false;
}

LIBCXXW_NAMESPACE_END
