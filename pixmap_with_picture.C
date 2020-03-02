/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/pixmap_with_picture.H"
#include "pixmap.H"
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

pixmap_with_pictureObj::pixmap_with_pictureObj(const ref<implObj> &pixmap_impl,
					       render_repeat repeat)
		: pixmapObj{pixmap_impl},
		  repeat{repeat},
		  icon_picture{pixmap_impl->create_picture()}
{
	icon_picture->repeat(repeat);
}

pixmap_with_pictureObj::~pixmap_with_pictureObj()=default;

const_picture pixmap_with_pictureObj::create_picture() const
{
	return icon_picture;
}

LIBCXXW_NAMESPACE_END
