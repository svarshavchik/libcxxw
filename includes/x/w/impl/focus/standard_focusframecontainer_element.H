/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_standard_focusframecontainer_element_h
#define x_w_impl_standard_focusframecontainer_element_h

#include <x/w/impl/focus/focusframecontainer_impl.H>
#include <x/w/focusable_container_owner.H>
#include <x/w/element.H>
#include <x/w/focusableobj.H>

LIBCXXW_NAMESPACE_START

//! Create a focusable_container_owner for a standard focusframe.

//! Mostly for internal library use. Creates a focusable_container_owner
//! for a standard focusframe implementation object.
//!
//! The focusable_container_owner gets registered as an owner for the specified
//! focusable implementation. This is typically some focusable object in the
//! the focus frame (either the given element itself, or some widget in the
//! the container), but whose public object is intentionally not the owner of
//! its focusable implementation object.
//!
//! The library uses it for buttons which contain non-focusable elements.
//! The button constructs a custom focusframecontainer that implements
//! the focusable interface, containing app-provided elements, and the
//! focusable_container becomes its public owner/object.

focusable_container_owner
create_focusframe_container_owner(const container_impl &parent_container,
				  const ref<focusframecontainer_implObj> &impl,
				  const element &e,
				  const focusable_impl &element_focusable_impl);

//! Create a container for a standard focusframe.

//! Constructs a container public object for a standard focusframe
//! implementation object.

container create_focusframe_container(const
				      ref<focusframecontainer_implObj> &impl,
				      const element &e);

LIBCXXW_NAMESPACE_END

#endif
