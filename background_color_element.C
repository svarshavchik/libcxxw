/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/background_color_element.H"

LIBCXXW_NAMESPACE_START

background_color_element_implObj
::background_color_element_implObj(const background_color &color)
	: color(color)
{
}

background_color_element_implObj::~background_color_element_implObj()=default;

void background_color_element_implObj::do_update(ONLY IN_THREAD,
						 const background_color
						 &new_color,
						 elementObj::implObj &e)
{
	color=new_color;
	color->initialize(IN_THREAD);
	background_color_element_width=0;
	background_color_element_height=0;
	set_background_color_for_element(IN_THREAD, e);
}

void background_color_element_implObj::theme_updated(ONLY IN_THREAD,
						     const defaulttheme
						     &new_theme,
						     elementObj::implObj &e)
{
	color->theme_updated(IN_THREAD, new_theme);
	background_color_element_width=0;
	background_color_element_height=0;
	set_background_color_for_element(IN_THREAD, e);
}

void background_color_element_implObj
::background_color_was_recalculated(ONLY IN_THREAD)
{
}

void background_color_element_implObj::initialize(ONLY IN_THREAD)
{
	color->initialize(IN_THREAD);
}

void background_color_element_implObj
::set_background_color_for_element(ONLY IN_THREAD,
				   elementObj::implObj &e)
{
	auto &pos=e.data(IN_THREAD).current_position;

	if (background_color_element_width == pos.width &&
	    background_color_element_height == pos.height)
		return;

	color=color->get_background_color_for(IN_THREAD, e);
	background_color_element_width=pos.width;
	background_color_element_height=pos.height;

	background_color_was_recalculated(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
