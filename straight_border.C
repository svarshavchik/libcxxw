/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "straight_border.H"

LIBCXXW_NAMESPACE_START

straight_borderObj::straight_borderObj(const ref<implObj> &impl)
	: elementObj(impl), impl(impl)
{
}

straight_borderObj::~straight_borderObj()=default;

LIBCXXW_NAMESPACE_END
