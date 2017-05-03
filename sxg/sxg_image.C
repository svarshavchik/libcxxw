/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "sxg/sxg_image.H"
#include "x/w/picture.H"
#include "x/w/pixmap.H"

LIBCXXW_NAMESPACE_START

sxg_imageObj::sxg_imageObj(const picture &rendered_picture,
			   const pixmap  &rendered_pixmap,
			   render_repeat repeat)
	: icon_imageObj(rendered_picture, rendered_pixmap, repeat)
{
}

sxg_imageObj::~sxg_imageObj()=default;

LIBCXXW_NAMESPACE_END
