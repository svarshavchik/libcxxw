/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "hotspot_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/rgb.H"
#include "messages.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::implObj::implObj(const container_impl &parent,
				   const new_listlayoutmanager &style)
	: listcontainer_impl_superclass_t
	  (

	   // Initialize the background colors
	   parent->container_element_impl()
	   .create_background_color(style.selected_color),
	   parent->container_element_impl()
	   .create_background_color(style.highlighted_color),
	   parent->container_element_impl()
	   .create_background_color(style.current_color),
	   parent)
{
	if (style.columns < 1)
		throw EXCEPTION(_("Cannot create a list with 0 columns"));
}

listcontainerObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
