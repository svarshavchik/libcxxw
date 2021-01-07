/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_always_visiblefwd_H
#define x_w_impl_always_visiblefwd_H

#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

//! A mixin for an always-visible container implementation object.

//! The template parameter is a container_elementObj or a subclass of it.
//! This is a mixin for an internal implementation of a container
//! that will always be visible.
//!
//! Overrides request_visibility_recursive(), and implements it by invoking
//! request_child_visibility_recursive(), so that show_all() or hide_all()
//! skip over this widget.
//!
//! The second template parameter defaults to true, and sets the
//! new element's initial requested visibility.

template<typename element_impl, bool initially_visible=true>
class always_visibleObj : public element_impl {

public:

	template<typename ...Args>
		always_visibleObj(Args && ...args);

	~always_visibleObj();

	void request_visibility_recursive(ONLY IN_THREAD, bool flag)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
