/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pixmap.H"
#include "x/w/picture.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

pixmapObj::pixmapObj(const ref<implObj> &impl)
	: drawableObj(impl),
	  impl(impl),
	  points_of_interest(impl->points_of_interest)
{
}

pixmapObj::~pixmapObj()=default;

LIBCXXW_NAMESPACE_END
