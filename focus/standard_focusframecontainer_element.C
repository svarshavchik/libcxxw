/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/standard_focusframecontainer_element.H"
#include "focus/focusframecontainer_element.H"
#include "container_element.H"
#include "container_visible_element.H"
#include "nonrecursive_visibility.H"
#include "background_color.H"

LIBCXXW_NAMESPACE_START

standard_focusframecontainer_element_t
create_standard_focusframe_container_element(const ref<containerObj::
					     implObj> &parent_container)
{
	return create_standard_focusframe_container_element
		(parent_container, background_colorptr());
}

standard_focusframecontainer_element_t
create_standard_focusframe_container_element(const ref<containerObj::
					     implObj> &parent_container,
					     const background_colorptr
					     &focusable_background_color)
{
	return standard_focusframecontainer_element_t
		::create(parent_container,
			 child_element_init_params{"focusframe@libcxx"},
			 focusable_background_color);
}

LIBCXXW_NAMESPACE_END
