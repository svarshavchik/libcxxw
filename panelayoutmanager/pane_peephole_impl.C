/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_impl.H"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "peephole/peephole_impl_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/theme_font_element.H"

LIBCXXW_NAMESPACE_START

pane_peepholeObj::implObj::implObj(const ref<pane_peephole_containerObj
				   ::implObj> &parent_container,
				   const child_element_init_params &init_params)
	: superclass_t{parent_container->label_theme_font(),
		parent_container, init_params},
	  parent_container{parent_container}
{
}

pane_peepholeObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
