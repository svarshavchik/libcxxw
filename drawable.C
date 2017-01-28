/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "drawable.H"
#include "x/w/pixmap.H"
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

screen drawableObj::get_screen()
{
	return impl->get_screen();
}

const_screen drawableObj::get_screen() const
{
	return impl->get_screen();
}

pixmap drawableObj::create_pixmap(dim_t width,
				  dim_t height)
{
	return impl->create_pixmap(width, height);
}

pixmap drawableObj::create_pixmap(dim_t width,
				  dim_t height,
				  const const_pictformat &drawable_pictformat)
{
	return impl->create_pixmap(width, height, drawable_pictformat);
}

pixmap drawableObj::create_pixmap(dim_t width,
				  dim_t height,
				  depth_t depth)
{
	return impl->create_pixmap(width, height, depth);
}

LIBCXXW_NAMESPACE_END
