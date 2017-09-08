/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "icon_images_vector.H"
#include "icon.H"

LIBCXXW_NAMESPACE_START

icon_images_vector::icon_images_vector(const std::vector<icon> &icon_images)
	: icon_images_thread_only(icon_images)
{
}

icon_images_vector::~icon_images_vector()=default;

void icon_images_vector::initialize(IN_THREAD_ONLY)
{
	for (auto &i:icon_images(IN_THREAD))
	{
		i=i->initialize(IN_THREAD);
	}
}

void icon_images_vector::theme_updated(IN_THREAD_ONLY,
				       const defaulttheme &new_theme)
{
	for (auto &i:icon_images(IN_THREAD))
	{
		i=i->theme_updated(IN_THREAD, new_theme);
	}
}

void icon_images_vector::resize(IN_THREAD_ONLY, dim_t w, dim_t h,
				icon_scale scale)
{
	for (auto &i:icon_images(IN_THREAD))
		i=i->resize(IN_THREAD, w, h, scale);
}

LIBCXXW_NAMESPACE_END
