/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "icon_image.H"
#include "x/w/picture.H"
#include "pixmap.H"

LIBCXXW_NAMESPACE_START

icon_imageObj::icon_imageObj(const const_pixmap &icon_pixmap,
			     render_repeat repeat)
	: icon_pixmap{icon_pixmap},
	  icon_picture{icon_pixmap->create_picture()},
	  repeat(repeat)
{
}

icon_imageObj::~icon_imageObj()=default;

LIBCXXW_NAMESPACE_END
