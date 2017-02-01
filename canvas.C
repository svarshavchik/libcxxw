/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "canvas.H"

LIBCXXW_NAMESPACE_START

canvasObj::canvasObj(const ref<implObj> &impl) : elementObj(impl),
						 impl(impl)
{
}

canvasObj::~canvasObj()=default;

LIBCXXW_NAMESPACE_END
