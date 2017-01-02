/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "drawable.H"
#include "x/w/pictformat.H"

LIBCXXW_NAMESPACE_START

drawableObj::implObj::implObj(xcb_drawable_t drawable_id,
			      const const_pictformat drawable_pictformat)
	: drawable_id(drawable_id),
	  drawable_pictformat(drawable_pictformat)
{
}

drawableObj::implObj::~implObj() noexcept=default;

LIBCXXW_NAMESPACE_END
