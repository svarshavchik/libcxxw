/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_impl.H"
#include "container_element.H"
#include "themedim_element_minoverride.H"
#include "background_color.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

progressbarObj::handlerObj::handlerObj(const ref<containerObj::implObj>
				       &parent_container,
				       const progressbar_config &config)
	: superclass_t{config.minimum_width, 0,
		parent_container,
		child_element_init_params{"progressbar@libcxx"}},
	  foreground_color{config.foreground_color}
{
}

progressbarObj::handlerObj::~handlerObj()=default;

const char *progressbarObj::handlerObj::label_theme_font() const
{
	return "progressbar";
}

color_arg progressbarObj::handlerObj::label_theme_color() const
{
	return foreground_color;
}

LIBCXXW_NAMESPACE_END