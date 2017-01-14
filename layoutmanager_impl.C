/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "layoutmanager.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

layoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

layoutmanagerObj::implObj::~implObj()=default;


LIBCXXW_NAMESPACE_END
