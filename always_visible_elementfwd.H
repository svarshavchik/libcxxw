/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef always_visible_elementfwd_H
#define always_visible_elementfwd_H

#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! A mixin for an always-visible element implementation object.

//! The template parameter is a child_elementObj or a subclass of it.
//! This is a mixin for an internal implementation of an element
//! that will always be visible.
//!
//! This is not for containers, but for individual display elements.
//! always_visibleObj is for containers.
//!
//! The constructor sets the visibility right off the bat (and forwards
//! its parameters to the superclass constructor).
//!
//! request_visibility gets overriden, and quietly does nothing.

template<typename element_impl>
class LIBCXX_HIDDEN always_visible_elementObj : public element_impl {

public:

	template<typename ...Args>
		always_visible_elementObj(Args && ...args);

	~always_visible_elementObj();

	void request_visibility(ONLY IN_THREAD, bool flag)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
