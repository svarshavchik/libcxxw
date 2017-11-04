/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_celltextobj_h
#define list_celltextobj_h

#include "listlayoutmanager/list_celltextfwd.H"
#include "listlayoutmanager/list_cellobj.H"
#include "richtext/richtext.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

struct LIBCXX_HIDDEN list_celltextObj : public list_cellObj,
					    public richtextObj {

	//! Calculate in advance.
	const bool is_empty;

	//! Constructor
	list_celltextObj(const richtextstring &string,
			     halign alignment,
			     dim_t word_wrap_width);

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
				const defaulttheme &initial_theme) override;

	//! Implement cell_is_empty().
	bool cell_is_empty() override;
};

LIBCXXW_NAMESPACE_END

#endif
