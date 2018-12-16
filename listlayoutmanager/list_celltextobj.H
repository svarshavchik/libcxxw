/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef list_celltextobj_h
#define list_celltextobj_h

#include "listlayoutmanager/list_celltextfwd.H"
#include "listlayoutmanager/list_cellobj.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtext_alteration_configfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

struct list_celltextObj : public list_cellObj,
			  public richtextObj {

	//! Constructor
	list_celltextObj(const richtext_alteration_config &richtext_alteration,
			 const richtextstring &string,
			 halign halignment,
			 valign valignment,
			 dim_t word_wrap_width);

	//! TODO

	//! These list_cells are stored in a vector in list_elementObj::implObj,
	//! and this is a reference to its superclass;
	const richtext_alteration_config &richtext_alteration;

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
			     const defaulttheme &initial_theme) override;

	//! Implement cell_theme_updated().
	void cell_theme_updated(ONLY IN_THREAD,
				const defaulttheme &initial_theme) override;

	//! Implement cell_is_separator().
	bool cell_is_separator() override;
};

LIBCXXW_NAMESPACE_END

#endif
