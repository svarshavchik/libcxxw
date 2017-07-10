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
#include "child_element.H"
#include "background_colorfwd.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! A typical implementation of a focusframecontainer mixin.

typedef ref<nonrecursive_visibilityObj<focusframecontainer_elementObj<
					       container_visible_elementObj<
						       container_elementObj<
							       child_elementObj>
						       >>>
	    > standard_focusframecontainer_element_t;

//! Create a standard focusframe container.

standard_focusframecontainer_element_t
create_standard_focusframe_container_element(const ref<containerObj::
					     implObj> &parent_container)
	LIBCXX_HIDDEN;

standard_focusframecontainer_element_t
create_standard_focusframe_container_element(const ref<containerObj::
					     implObj> &parent_container,
					     const background_colorptr
					     &focusable_background_color)
	LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
