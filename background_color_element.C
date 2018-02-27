/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "background_color_element.H"

LIBCXXW_NAMESPACE_START

background_color_element_implObj
::background_color_element_implObj(const background_color &color)
	: color(color)
{
}

background_color_element_implObj::~background_color_element_implObj()=default;

void background_color_element_implObj::do_update(IN_THREAD_ONLY,
						 const background_color
						 &new_color)
{
	color=new_color;
	color->initialize(IN_THREAD);
}

void background_color_element_implObj::theme_updated(IN_THREAD_ONLY,
						     const defaulttheme
						     &new_theme)
{
	color->theme_updated(IN_THREAD, new_theme);

	// The background_color_element template will immediately call
	// set_background_color_for_element().
	background_color_element_width=0;
	background_color_element_height=0;
}

void background_color_element_implObj
::background_color_was_recalculated(IN_THREAD_ONLY)
{
}

void background_color_element_implObj::initialize(IN_THREAD_ONLY)
{
	color->initialize(IN_THREAD);
}

void background_color_element_implObj
::set_background_color_for_element(IN_THREAD_ONLY,
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
