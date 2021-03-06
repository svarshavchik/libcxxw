/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/container_element.H"

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
