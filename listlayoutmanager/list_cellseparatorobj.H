/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_cellseparatorobj_h
#define list_cellseparatorobj_h

#include "listlayoutmanager/list_cellseparatorfwd.H"
#include "listlayoutmanager/list_cellobj.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

struct LIBCXX_HIDDEN list_cellseparatorObj : public list_cellObj {

	//! Constructor
	list_cellseparatorObj();

	//! Implement cell_get_metrics().
	std::pair<metrics::axis, metrics::axis>
		cell_get_metrics(ONLY IN_THREAD, dim_t preferred_width)
		override;

	//! Implement cell_redraw().
	void cell_redraw(ONLY IN_THREAD,
			 element_drawObj &draw,
			 const draw_info &di,
			 bool draw_as_disabled,
			 richtext_draw_boundaries &boundaries) override;

	//! Implement cell_initialize().
	void cell_initialize(ONLY IN_THREAD,
			     const defaulttheme &initial_theme) override;

	//! Implement cell_theme_updated().
	void cell_theme_updated(ONLY IN_THREAD,
				const defaulttheme &initial_theme) override;

	//! Implement cell_is_separator().
	bool cell_is_separator() override;
};

LIBCXXW_NAMESPACE_END

#endif
