/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "icon.H"
#include "icon_image.H"
#include "pixmap.H"
#include "defaulttheme.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

iconObj::iconObj(const const_icon_image &image)
	: image(image)
{
}

iconObj::~iconObj()=default;

icon iconObj::initialize(IN_THREAD_ONLY)
{
	auto theme=image->icon_pixmap->impl->get_screen()->impl
		->current_theme.get();

	return theme_updated(IN_THREAD, theme);
}

icon iconObj::theme_updated(IN_THREAD_ONLY,
			    const defaulttheme &new_theme)
{
	return icon(this);
}

icon iconObj::resizemm(IN_THREAD_ONLY,
		       const dim_arg &,
		       const dim_arg &)
{
	return icon(this);
}

icon iconObj::resize(IN_THREAD_ONLY, dim_t w, dim_t h, icon_scale scale)
{
	return icon(this);
}

LIBCXXW_NAMESPACE_END
