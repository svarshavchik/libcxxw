/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "actionbutton.H"
#include "container_elementfwd.H"

LIBCXXW_NAMESPACE_START

actionbuttonObj::implObj::implObj(const ref<containerObj::implObj> &container)
	: container_elementObj<child_elementObj>(container)
{
}

actionbuttonObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
