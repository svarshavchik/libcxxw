/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "container_element.H"
#include "container_visible_element.H"

LIBCXXW_NAMESPACE_START

buttonObj::implObj::implObj(const ref<containerObj::implObj> &container)
	: superclass_t(container)
{
}

buttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
