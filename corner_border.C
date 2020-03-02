/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "corner_border.H"

LIBCXXW_NAMESPACE_START

corner_borderObj::corner_borderObj(const ref<implObj> &impl)
	: elementObj(impl), impl(impl)
{
}

corner_borderObj::~corner_borderObj()=default;

LIBCXXW_NAMESPACE_END
