/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/factory.H"
#include "container.H"
#include "messages.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

factoryObj::factoryObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

factoryObj::~factoryObj()=default;

void factoryObj::created_internally(const element &e)
{
	created(e);
}

LIBCXXW_NAMESPACE_END
