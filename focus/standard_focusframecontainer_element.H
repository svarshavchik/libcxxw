/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef standard_focusframecontainer_element_h
#define standard_focusframecontainer_element_h

#include "focus/focusframecontainer_elementfwd.H"
#include "container_elementfwd.H"
#include "container_visible_elementfwd.H"
#include "nonrecursive_visibilityfwd.H"
#include "always_visiblefwd.H"
#include "child_element.H"
#include "background_colorfwd.H"
#include "x/w/border_arg.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

#define FOCUSFRAME_ID "focusframe@libcxx.com"

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
create_always_visible_focusframe(const ref<containerObj::implObj>
				 &parent_container)
	LIBCXX_HIDDEN;

//! Create an always_visibleObj focus frame.

always_visible_focusframe_ref_t
create_always_visible_focusframe(const ref<containerObj::implObj>
				 &parent_container,
				 const border_arg &focusoff_border,
				 const border_arg &focuson_border)
	LIBCXX_HIDDEN;

//! Create an always_visibleObj focus frame.

always_visible_focusframe_ref_t
create_always_visible_focusframe(const ref<containerObj::implObj>
				 &parent_container,
				 const border_arg &focusoff_border,
				 const border_arg &focuson_border,
				 const background_colorptr
				 &focusable_background_color)
	LIBCXX_HIDDEN;



//! Create an nonrecursive_visibilityObj focus frame.

nonrecursive_visibility_focusframe_ref_t
create_nonrecursive_visibility_focusframe(const ref<containerObj::implObj>
					  &parent_container)
	LIBCXX_HIDDEN;

//! Create an nonrecursive_visibilityObj focus frame.

nonrecursive_visibility_focusframe_ref_t
create_nonrecursive_visibility_focusframe(const ref<containerObj::implObj>
					  &parent_container,
					  const border_arg &focusoff_border,
					  const border_arg &focuson_border)
	LIBCXX_HIDDEN;

//! Create an nonrecursive_visibilityObj focus frame.

nonrecursive_visibility_focusframe_ref_t
create_nonrecursive_visibility_focusframe(const ref<containerObj::implObj>
					  &parent_container,
					  const border_arg &focusoff_border,
					  const border_arg &focuson_border,
					  const background_colorptr
					  &focusable_background_color)
	LIBCXX_HIDDEN;


LIBCXXW_NAMESPACE_END

#endif
