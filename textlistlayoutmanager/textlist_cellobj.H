/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlist_cellobj_h
#define textlist_cellobj_h

#include "textlistlayoutmanager/textlist_cellfwd.H"
#include "textlistlayoutmanager/textlist_impl.H"
#include "connection_threadfwd.H"
#include "element_draw.H"
#include "draw_infofwd.H"
#include "defaultthemefwd.H"
#include "richtext/richtext_draw_boundariesfwd.H"
#include "x/w/metrics/axis.H"

LIBCXXW_NAMESPACE_START

//! Base class for individual cells in a \ref textlistlayoutmanager "textlistlayoutmanager".
struct LIBCXX_HIDDEN textlist_cellObj : virtual public obj {

	//! Our iterator in column_widths
	list_column_widths_t::iterator column_iterator;

	//! Cached height.
	dim_t height;

	//! Return this cell's metrics.
	virtual std::pair<metrics::axis, metrics::axis>
		cell_get_metrics(IN_THREAD_ONLY, dim_t preferred_width,
			    bool visible)=0;

	//! Draw this cell's contents.
	virtual void cell_redraw(IN_THREAD_ONLY,
			    element_drawObj &draw,
			    const draw_info &di,
			    bool draw_as_disabled,
			    const richtext_draw_boundaries &boundaries)=0;

	//! Initialize this cell, in the connection thread.
	virtual void cell_initialize(IN_THREAD_ONLY,
				     const defaulttheme &initial_theme)=0;

	//! Update this cell to a new theme.
	virtual void cell_theme_updated(IN_THREAD_ONLY,
					const defaulttheme &initial_theme)=0;

	//! Whether this cell is considered "empty".

	//! If all cells in an item are "empty", the
	//! \ref "textlistlayoutmanager" draws a separator border.
	virtual bool cell_is_empty()=0;
};

LIBCXXW_NAMESPACE_END

#endif
