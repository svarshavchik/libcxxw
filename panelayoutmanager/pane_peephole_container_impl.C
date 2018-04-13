/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "always_visible.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

pane_peephole_containerObj::implObj::implObj(const container_impl
					     &parent_container,
					     const child_element_init_params
					     &init_params)
	: superclass_t{parent_container, init_params}
{
}

pane_peephole_containerObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
