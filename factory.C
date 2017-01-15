/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/factory.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

factoryObj::factoryObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

factoryObj::~factoryObj()=default;

LIBCXXW_NAMESPACE_END
