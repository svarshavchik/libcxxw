/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef nonrecursive_visibility_fwd_H
#define nonrecursive_visibility_fwd_H

#include "x/w/namespace.H"
#include "x/w/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! An element that does not implement recursive visibility changes.

//! A mixin for implementing a container of some kind that controls the
//! visibility of the objects in the container.
//!
//! The template parameter is a containerObj::implObj, or its subclass.
//! Overrides request_visibility_recursive() by calling request_visibility(),
//! bypassing the default container implementation of
//! request_visibility_recursive().

template<typename container_impl>
class LIBCXX_HIDDEN nonrecursive_visibilityObj : public container_impl {

 public:
	using container_impl::container_impl;

	//! Destructor

	~nonrecursive_visibilityObj();

	//! Override request_visibility_recursive().

	//! The container controls the visibility of the objects in the
	//! container, manually.

	void request_visibility_recursive(ONLY IN_THREAD, bool flag)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
