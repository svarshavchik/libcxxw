/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/icon.H"
#include "x/w/impl/pixmap_with_picture.H"
#include "pixmap.H"
#include "defaulttheme.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

iconObj::iconObj(const pixmap_with_picture &image)
	: image(image)
{
}

iconObj::~iconObj()=default;

icon iconObj::initialize(ONLY IN_THREAD)
{
	auto theme=image->impl->get_screen()->impl->current_theme.get();

	return theme_updated(IN_THREAD, theme);
}

icon iconObj::theme_updated(ONLY IN_THREAD,
			    const const_defaulttheme &new_theme)
{
	return icon(this);
}

icon iconObj::resize(ONLY IN_THREAD, dim_t w, dim_t h, icon_scale scale)
{
	return icon(this);
}

LIBCXXW_NAMESPACE_END
