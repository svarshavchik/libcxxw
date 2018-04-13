/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "container_element.H"
#include "container_visible_element.H"

LIBCXXW_NAMESPACE_START

buttonObj::implObj::implObj(const container_impl &container,
			    const child_element_init_params &init_params)
	: superclass_t{container, init_params}
{
}

buttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
