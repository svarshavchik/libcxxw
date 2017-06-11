/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_listcontainer_impl.H"
#include "reference_font_element.H"
#include "x/w/listlayoutmanager.H"

LIBCXXW_NAMESPACE_START

static const char default_list_font[]="list";

const char *peepholed_listcontainerObj::implObj::label_theme_font() const
{
	return default_list_font;
}

peepholed_listcontainerObj::implObj
::implObj(const ref<containerObj::implObj> &parent,
	  const new_listlayoutmanager &style)
	: superclass_t(default_list_font, parent, style),
	  rows(style.rows)
{
}

peepholed_listcontainerObj::implObj::~implObj()=default;

void peepholed_listcontainerObj::implObj::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	update_peephole_metrics(IN_THREAD);
}

void peepholed_listcontainerObj::implObj
::horizvert_updated(IN_THREAD_ONLY)
{
	superclass_t::horizvert_updated(IN_THREAD);
	update_peephole_metrics(IN_THREAD);
}

dim_t peepholed_listcontainerObj::implObj::rowsize(IN_THREAD_ONLY) const
{
	return dim_t::truncate
		(reference_font_element<>::font_height(IN_THREAD)
		 + themedim_element<listcontainer_dim_v>
		 ::pixels(IN_THREAD)
		 + themedim_element<listcontainer_dim_v>
		 ::pixels(IN_THREAD));
}

void peepholed_listcontainerObj::implObj
::update_peephole_metrics(IN_THREAD_ONLY)
{
	auto h=dim_t::truncate(rowsize(IN_THREAD) *
			       dim_t{dim_t::truncate(rows)});

	//! We keep our horizontal metrics, and override the vertical
	//! metrics to the fixed height.

	auto hv=get_horizvert(IN_THREAD);

	container->get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      hv->horiz,
				      {h, h, h});
}

LIBCXXW_NAMESPACE_END
