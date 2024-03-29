/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_container_visible_element_H
#define x_w_impl_container_visible_element_H

#include <x/w/impl/container_visible_elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Make the whole container visible, not just its child widget.

//! The template parameter is a subclass of container_elementObj.
//! Implements ensured_visibility_of_child_element() by invoking
//! ensure_entire_visibility(). This is used by widgets that
//! are containers with focusable elements, and when the focus widget
//! wants to insure its visibility the entire widgets wants to
//! ensure its visibility.

template<typename container_impl>
void container_visible_elementObj<container_impl>
::ensured_visibility_of_child_element(ONLY IN_THREAD,
				      elementObj::implObj &e,
				      const element_visibility_t &v)
{
	this->ensure_entire_visibility(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
