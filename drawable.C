/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "drawable.H"
#include "x/w/picture.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

drawableObj::drawableObj(const ref<implObj> &impl)
	: impl(impl)
{
}

drawableObj::~drawableObj()=default;

const_pictformat drawableObj::get_pictformat() const
{
	return impl->drawable_pictformat;
}

picture drawableObj::create_picture()
{
	return impl->create_picture();
}

LIBCXXW_NAMESPACE_END
