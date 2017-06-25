/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "icon_images_vectorobj.H"
#include "icon.H"

LIBCXXW_NAMESPACE_START

icon_images_vectorObj::icon_images_vectorObj(const std::vector<icon>
					     &icon_images)
	: icon_images_thread_only(icon_images)
{
}

icon_images_vectorObj::~icon_images_vectorObj()=default;

void icon_images_vectorObj::initialize(IN_THREAD_ONLY)
{
	for (auto &i:icon_images(IN_THREAD))
	{
		i=i->initialize(IN_THREAD);
	}
}

void icon_images_vectorObj::theme_updated(IN_THREAD_ONLY,
					  const defaulttheme &new_theme)
{
	for (auto &i:icon_images(IN_THREAD))
	{
		i=i->theme_updated(IN_THREAD, new_theme);
	}
}

LIBCXXW_NAMESPACE_END
