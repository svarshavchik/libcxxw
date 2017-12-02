/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "icon_image.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"

LIBCXXW_NAMESPACE_START

icon_imageObj::icon_imageObj(const picture &icon_pictureArg,
			     const pixmap &icon_pixmap,
			     render_repeat repeat)
	: icon_picture(icon_pictureArg),
	  icon_pixmap(icon_pixmap),
	  repeat(repeat)
{
	icon_pictureArg->repeat(repeat);
}

icon_imageObj::~icon_imageObj()=default;

LIBCXXW_NAMESPACE_END
