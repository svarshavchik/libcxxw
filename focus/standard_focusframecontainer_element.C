/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/standard_focusframecontainer_element.H"
#include "focus/focusframecontainer_element.H"
#include "container_element.H"
#include "container_visible_element.H"
#include "always_visible.H"
#include "nonrecursive_visibility.H"
#include "background_color.H"

LIBCXXW_NAMESPACE_START

always_visible_focusframe_t
create_always_visible_focusframe(const ref<containerObj::implObj>
				 &parent_container)
{
	return create_always_visible_focusframe
		(parent_container,
		 "inputfocusoff_border", "inputfocuson_border");
}

always_visible_focusframe_t
create_always_visible_focusframe(const ref<containerObj::implObj>
				 &parent_container,
				 const border_arg &focusoff_border,
				 const border_arg &focuson_border)
{
	return create_always_visible_focusframe
		(parent_container,
		 focusoff_border, focuson_border,
		 background_colorptr());
}

always_visible_focusframe_t
create_always_visible_focusframe(const ref<containerObj::implObj>
				 &parent_container,
				 const border_arg &focusoff_border,
				 const border_arg &focuson_border,
				 const background_colorptr
				 &focusable_background_color)
{
	auto e=always_visible_focusframe_t
		::create(focusoff_border,
			 focuson_border,
			 parent_container,
			 child_element_init_params{"focusframe@libcxx.com"},
			 focusable_background_color);

	e->request_visibility(true);

	return e;
}

nonrecursive_visibility_focusframe_t
create_nonrecursive_visibility_focusframe(const ref<containerObj::implObj>
					  &parent_container)
{
	return create_nonrecursive_visibility_focusframe
		(parent_container,
		 "inputfocusoff_border", "inputfocuson_border");
}

nonrecursive_visibility_focusframe_t
create_nonrecursive_visibility_focusframe(const ref<containerObj::implObj>
					  &parent_container,
					  const border_arg &focusoff_border,
					  const border_arg &focuson_border)
{
	return create_nonrecursive_visibility_focusframe
		(parent_container,
		 focusoff_border, focuson_border,
		 background_colorptr());
}

nonrecursive_visibility_focusframe_t
create_nonrecursive_visibility_focusframe(const ref<containerObj::implObj>
					  &parent_container,
					  const border_arg &focusoff_border,
					  const border_arg &focuson_border,
					  const background_colorptr
					  &focusable_background_color)
{
	auto e=nonrecursive_visibility_focusframe_t
		::create(focusoff_border,
			 focuson_border,
			 parent_container,
			 child_element_init_params{"focusframe@libcxx.com"},
			 focusable_background_color);

	return e;
}

LIBCXXW_NAMESPACE_END
