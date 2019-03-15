/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_impl.H"
#include "x/w/text_param_literals.H"
#include "x/w/impl/container_element.H"
#include "themedim_element_minoverride.H"
#include "x/w/impl/background_color.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

progressbarObj::handlerObj::handlerObj(const container_impl
				       &parent_container,
				       const progressbar_config &config)
	: superclass_t{config.minimum_width, 0,
		parent_container,
		child_element_init_params{"progressbar@libcxx.com"}},
	  foreground_color{config.foreground_color}
{
}

progressbarObj::handlerObj::~handlerObj()=default;

font_arg progressbarObj::handlerObj::label_theme_font() const
{
	return "progressbar"_theme_font;
}

color_arg progressbarObj::handlerObj::label_theme_color() const
{
	return foreground_color;
}

LIBCXXW_NAMESPACE_END
