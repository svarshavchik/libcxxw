/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "switchfactory_impl.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

switchfactoryObj::implObj::implObj(const switchlayoutmanager &lm)
	: lm(lm)
{
}

switchfactoryObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
