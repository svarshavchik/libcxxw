/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_cellimageobj_h
#define list_cellimageobj_h

#include "listlayoutmanager/list_cellimagefwd.H"
#include "listlayoutmanager/list_cellobj.H"
#include "iconfwd.H"
#include "icon_images_vector.H"
#include "connection_threadfwd.H"

#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object for an image cell in a \ref listlayoutmanager "listlayoutmanager".

struct LIBCXX_HIDDEN list_cellimageObj : public list_cellObj,
					     public icon_images_vector {

	//! Image's alignment.

	const halign alignment;

	//! Which image number is currently drawn.
	mpobj<size_t> n=0;

	//! Constructor.

	list_cellimageObj(const std::vector<icon> &images,
			      halign alignment);

	//! Implement cell_get_metrics().
	std::pair<metrics::axis, metrics::axis>
		cell_get_metrics(IN_THREAD_ONLY, dim_t preferred_width,
				 bool visible) override;

	//! Implement cell_redraw().
	void cell_redraw(IN_THREAD_ONLY,
			 element_drawObj &draw,
			 const draw_info &di,
			 bool draw_as_disabled,
			 const richtext_draw_boundaries &boundaries) override;

	//! Implement cell_initialize().
	void cell_initialize(IN_THREAD_ONLY,
			     const defaulttheme &initial_theme) override;

	//! Implement cell_theme_updated().
	void cell_theme_updated(IN_THREAD_ONLY,
				const defaulttheme &new_theme) override;

	//! Implement cell_is_separator().
	bool cell_is_separator() override;
};

LIBCXXW_NAMESPACE_END

#endif
