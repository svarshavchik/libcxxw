/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "background_color_element.H"

LIBCXXW_NAMESPACE_START

background_color_element_implObj
::background_color_element_implObj(const background_colorptr &color)
	: color(color)
{
}

background_color_element_implObj::~background_color_element_implObj()=default;

void background_color_element_implObj::update(IN_THREAD_ONLY,
					      const background_colorptr &new_color)
{
	color=new_color;
	if (color)
		color->initialize(IN_THREAD);
}

void background_color_element_implObj::theme_updated(IN_THREAD_ONLY,
						     const defaulttheme
						     &new_theme)
{
	if (color)
		color->theme_updated(IN_THREAD, new_theme);
}

void background_color_element_implObj::initialize(IN_THREAD_ONLY)
{
	if (color)
		color->initialize(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
