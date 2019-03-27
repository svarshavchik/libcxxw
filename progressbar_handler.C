/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_impl.H"
#include "x/w/impl/container_element.H"
#include "themedim_element_minoverride.H"
#include "x/w/impl/background_color.H"
#include "x/w/progressbar_appearance.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

progressbarObj::handlerObj::handlerObj(const container_impl
				       &parent_container,
				       const const_progressbar_appearance
				       &appearance)
	: superclass_t{appearance->minimum_width, 0,
		parent_container,
		child_element_init_params{"progressbar@libcxx.com"}},
	  label_font{appearance->label_font},
	  foreground_color{appearance->foreground_color}
{
}

progressbarObj::handlerObj::~handlerObj()=default;

font_arg progressbarObj::handlerObj::label_theme_font() const
{
	return label_font;
}

color_arg progressbarObj::handlerObj::label_theme_color() const
{
	return foreground_color;
}

LIBCXXW_NAMESPACE_END
