/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/menufactory.H"

LIBCXXW_NAMESPACE_START

menufactoryObj::menufactoryObj(const ref<containerObj::implObj> &container_impl)
	: factoryObj(container_impl)
{
}

menufactoryObj::~menufactoryObj()=default;

LIBCXXW_NAMESPACE_END
