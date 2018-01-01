/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "always_visible.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

pane_peephole_containerObj::implObj::implObj(const ref<containerObj::implObj>
					     &parent_container)
	: superclass_t{parent_container}
{
}

pane_peephole_containerObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
