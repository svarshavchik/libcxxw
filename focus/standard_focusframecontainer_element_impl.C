/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "x/w/impl/focus/focusframecontainer_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "x/w/impl/background_color.H"

LIBCXXW_NAMESPACE_START

always_visible_focusframe_ref_t
create_always_visible_focusframe_impl(const container_impl &parent_container)
{
	return create_always_visible_focusframe_impl
		(parent_container,
		 "inputfocusoff_border", "inputfocuson_border");
}

always_visible_focusframe_ref_t
create_always_visible_focusframe_impl(const container_impl &parent_container,
				 const border_arg &focusoff_border,
				 const border_arg &focuson_border,
				 const std::optional<color_arg> &bgcolor)
{
	auto e=always_visible_focusframe_ref_t
		::create(focusoff_border,
			 focuson_border,
			 parent_container,
			 child_element_init_params{FOCUSFRAME_SCRATCH_BUFFER_ID,
				 {},
					 bgcolor});

	return e;
}

nonrecursive_visibility_focusframe_ref_t
create_nonrecursive_visibility_focusframe_impl(const container_impl
					       &parent_container)
{
	return create_nonrecursive_visibility_focusframe_impl
		(parent_container,
		 "inputfocusoff_border", "inputfocuson_border");
}

nonrecursive_visibility_focusframe_ref_t
create_nonrecursive_visibility_focusframe_impl
(const container_impl &parent_container,
 const border_arg &focusoff_border,
 const border_arg &focuson_border,
 const std::optional<color_arg> &bgcolor)
{
	auto e=nonrecursive_visibility_focusframe_ref_t
		::create(focusoff_border,
			 focuson_border,
			 parent_container,
			 child_element_init_params{FOCUSFRAME_SCRATCH_BUFFER_ID,
				 {},
					 bgcolor});

	return e;
}

LIBCXXW_NAMESPACE_END