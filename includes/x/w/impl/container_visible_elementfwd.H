/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_container_visible_elementfwd_H
#define x_w_impl_container_visible_elementfwd_H

#include <x/w/connection_threadfwd.H>
#include <x/w/rectanglefwd.H>
#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

template<typename container_impl>
class LIBCXX_PUBLIC container_visible_elementObj;

//! Make the whole container visible, not just its child element.

//! The template parameter is a subclass of container_elementObj.
//! Implements ensured_visibility_of_child_element() by invoking
//! ensure_entire_visibility(). This is used by display elements that
//! are containers with focusable elements, and when the focus element
//! wants to ensure its visibility the entire display element wants to
//! ensure its visibility.

template<typename container_impl>
class container_visible_elementObj : public container_impl {

public:

	using container_impl::container_impl;

	void ensured_visibility_of_child_element(ONLY IN_THREAD,
						 elementObj::implObj &e,
						 const rectangle &r) override;
};

LIBCXXW_NAMESPACE_END

#endif
