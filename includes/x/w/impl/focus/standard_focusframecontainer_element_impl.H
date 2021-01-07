/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_standard_focusframecontainer_element_impl_h
#define x_w_impl_standard_focusframecontainer_element_impl_h

#include <x/w/impl/focus/focusframecontainer_element.H>
#include <x/w/impl/container_elementfwd.H>
#include <x/w/impl/container_visible_elementfwd.H>
#include <x/w/impl/nonrecursive_visibilityfwd.H>
#include <x/w/impl/always_visiblefwd.H>
#include <x/w/impl/child_element.H>
#include <x/w/impl/background_colorfwd.H>
#include <x/w/focus_border_appearancefwd.H>
#include <x/ref.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Scratch buffer id used by focus frame elements

#define FOCUSFRAME_SCRATCH_BUFFER_ID "focusframe@libcxx.com"

//! A typical implementation of a focusframecontainer always_visibleObj mixin.

typedef always_visibleObj<focusframecontainer_elementObj<
				  container_visible_elementObj<
					  container_elementObj<
						  child_elementObj>
					  >>> always_visible_focusframe_t;

//! A typical implementation of a focusframecontainer always_visibleObj mixin.

typedef ref<always_visible_focusframe_t> always_visible_focusframe_ref_t;

//! A typical implementation of a focusframecontainer nonrecursive_visibilityObj mixin.

typedef nonrecursive_visibilityObj<focusframecontainer_elementObj<
					       container_visible_elementObj<
						       container_elementObj<
							       child_elementObj>
						       >>
				   > nonrecursive_visibility_focusframe_t;

//! A typical implementation of a focusframecontainer nonrecursive_visibilityObj mixin.

typedef ref<nonrecursive_visibility_focusframe_t
	    > nonrecursive_visibility_focusframe_ref_t;

//! Create an always_visibleObj focus frame.

always_visible_focusframe_ref_t
create_always_visible_focusframe_impl(const container_impl &parent_container,
				      const const_focus_border_appearance &,
				      const dim_arg &hpad,
				      const dim_arg &vpad,
				      const std::optional<color_arg> &bgcolor
				      ={});

//! Create an nonrecursive_visibilityObj focus frame.

nonrecursive_visibility_focusframe_ref_t
create_nonrecursive_visibility_focusframe_impl(const container_impl
					       &parent_container,
					       const const_focus_border_appearance &,
					       const dim_arg &hpad,
					       const dim_arg &vpad,
					       const std::optional<color_arg>
					       &bgcolor={});

LIBCXXW_NAMESPACE_END

#endif
