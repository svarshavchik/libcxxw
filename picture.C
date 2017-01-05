/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "picture.H"
#include "pictformat.H"

LIBCXXW_NAMESPACE_START

pictureObj::pictureObj(const ref<implObj> &impl)
	: impl(impl)
{
}

pictureObj::~pictureObj()=default;

LIBCXXW_NAMESPACE_END
