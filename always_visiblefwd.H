/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef always_visiblefwd_H
#define always_visiblefwd_H

#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! A mixin for an always-visible implementation object.

//! The template parameter is a container_elementObj or a subclass of it.
//! This is a mixin for an internal implementation of a display element
//! that will always be visible.
//!
//! Overrides request_visibility_recursive(), and implements it by invoking
//! request_child_visibility_recursive(), so that show_all() or hide_all()
//! skip over this display element.

template<typename element_impl>
class LIBCXX_HIDDEN always_visibleObj : public element_impl {

public:

	using element_impl::element_impl;

	void request_visibility_recursive(IN_THREAD_ONLY, bool flag)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
