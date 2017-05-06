/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "icon.H"
#include "icon_image.H"

LIBCXXW_NAMESPACE_START

iconObj::iconObj(const const_icon_image &image)
	: image(image)
{
}

iconObj::~iconObj()=default;

icon iconObj::initialize(IN_THREAD_ONLY)
{
	return theme_updated(IN_THREAD);
}

icon iconObj::theme_updated(IN_THREAD_ONLY)
{
	return icon(this);
}

LIBCXXW_NAMESPACE_END
