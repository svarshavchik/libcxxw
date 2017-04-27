/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "button.H"
#include "container_elementfwd.H"

LIBCXXW_NAMESPACE_START

buttonObj::implObj::implObj(const ref<containerObj::implObj> &container)
	: container_elementObj<child_elementObj>(container)
{
}

buttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
