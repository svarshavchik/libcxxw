/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_cellobj_h
#define list_cellobj_h

#include "listlayoutmanager/list_cellfwd.H"
#include "listlayoutmanager/list_element_impl.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/impl/draw_infofwd.H"
#include "x/w/defaultthemefwd.H"
#include "richtext/richtext_draw_boundariesfwd.H"
#include "x/w/metrics/axis.H"

LIBCXXW_NAMESPACE_START

//! Base class for individual cells in a \ref listlayoutmanager "listlayoutmanager".
struct LIBCXX_HIDDEN list_cellObj : virtual public obj {

	//! Our iterator in column_widths
	list_column_widths_t::iterator column_iterator;

	//! Cached height.
	dim_t height;

	//! Return this cell's metrics.
	virtual std::pair<metrics::axis, metrics::axis>
		cell_get_metrics(ONLY IN_THREAD, dim_t preferred_width)=0;

	//! Draw this cell's contents.
	virtual void cell_redraw(ONLY IN_THREAD,
				 element_drawObj &draw,
				 const draw_info &di,
				 clip_region_set &clipped,
				 bool draw_as_disabled,
				 richtext_draw_boundaries &boundaries)=0;

	//! Initialize this cell, in the connection thread.
	virtual void cell_initialize(ONLY IN_THREAD,
				     const defaulttheme &initial_theme)=0;

	//! Update this cell to a new theme.
	virtual void cell_theme_updated(ONLY IN_THREAD,
					const defaulttheme &initial_theme)=0;

	//! Whether this cell is considered "empty".

	//! If all cells in an item are "empty", the
	//! \ref "listlayoutmanager" draws a separator border.
	virtual bool cell_is_separator()=0;
};

LIBCXXW_NAMESPACE_END

#endif
