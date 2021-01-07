/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_cellimageobj_h
#define list_cellimageobj_h

#include "listlayoutmanager/list_cellimagefwd.H"
#include "listlayoutmanager/list_cellobj.H"
#include "x/w/impl/iconfwd.H"
#include "icon_images_vector.H"
#include "x/w/connection_threadfwd.H"

#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object for an image cell in a \ref listlayoutmanager "listlayoutmanager".

struct list_cellimageObj : public list_cellObj,
			   public icon_images_vector {

	//! Image's alignment.

	const halign halignment;

	//! Which image number is currently drawn.
	mpobj<size_t> n=0;

	//! Constructor.

	list_cellimageObj(const std::vector<icon> &images,
			  halign halignment,
			  valign valignment);

	//! Implement cell_get_metrics().
	std::pair<metrics::axis, metrics::axis>
		cell_get_metrics(ONLY IN_THREAD, dim_t preferred_width)
		override;

	//! Implement cell_redraw().
	void cell_redraw(ONLY IN_THREAD,
			 element_drawObj &draw,
			 const draw_info &di,
			 clip_region_set &clipped,
			 bool draw_as_disabled,
			 richtext_draw_boundaries &boundaries) override;

	//! Implement cell_initialize().
	void cell_initialize(ONLY IN_THREAD,
			     const const_defaulttheme &initial_theme) override;

	//! Implement cell_theme_updated().
	void cell_theme_updated(ONLY IN_THREAD,
				const const_defaulttheme &new_theme)
		override;

	//! Implement cell_is_separator().
	bool cell_is_separator() override;
};

LIBCXXW_NAMESPACE_END

#endif
