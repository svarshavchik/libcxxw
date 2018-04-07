/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pagefactory_impl.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

pagefactoryObj::implObj::implObj(const pagelayoutmanager &lm)
	: lm(lm)
{
}

pagefactoryObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
