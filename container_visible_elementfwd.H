/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef container_visible_elementfwd_H
#define container_visible_elementfwd_H

#include "x/w/connection_threadfwd.H"
#include "x/w/rectanglefwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! Make the whole container visible, not just its child element.

//! The template parameter is a subclass of container_elementObj.
//! Implements ensured_visibility_of_child_element() by invoking
//! ensure_entire_visibility(). This is used by display elements that
//! are containers with focusable elements, and when the focus element
//! wants to insure its visibility the entire display elements wants to
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
