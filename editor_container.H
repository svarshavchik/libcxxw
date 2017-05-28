/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef editor_container_h
#define editor_container_h

#include "editor_containerfwd.H"
#include "editor_containerobj.H"
#include "x/w/containerobj.H"
#include "x/w/text_paramfwd.H"
#include "x/w/input_field_config.H"
#include "scrollbar/scrollbarfwd.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Create a \ref editor_container "pepehole container with an editor element".

//! This creates the peephole container with an editor element inside it.
//! The peephole will scroll the editor element to keep its cursor in view.
//!
//! Returns the editor container and the scrollbars for it. The scrollbars
//! are created, but not inserted into their layout manager. The caller
//! is responsible for it.
//!
//! parent_container is the container for the editor element. When creating
//! the input field this is going to be the focus frame around the input
//! field.
//!
//! scrollbars_container is the container for the scrollbars. This is going
//! to be the input field container, a grid layout manager that will have
//! the focusframe with a peephole, and the two scrollbars.

std::tuple<editor_container, scrollbar, scrollbar>
create_editor_container(const ref<containerObj::implObj> &parent_container,
			const ref<containerObj::implObj> &scrollbars_container,
			const text_param &initial_contents,
			const input_field_config &config) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
