/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peepholed_attachedto_container_impl.H"
#include "reference_font_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/container_element.H"

LIBCXXW_NAMESPACE_START

peepholed_attachedto_containerObj::implObj
::implObj(const container_impl &parent,
	  const child_element_init_params &init_params)
	: superclass_t{theme_font{ parent->container_element_impl()
			.label_theme_font() },
		parent, init_params}
{
}

peepholed_attachedto_containerObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END