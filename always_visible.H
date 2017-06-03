/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef always_visible_H
#define always_visible_H

#include "always_visiblefwd.H"

LIBCXXW_NAMESPACE_START

//! Override request_visibility_recursive()

//! Forward to request_child_visibility_recursive().

template<typename element_impl>
void always_visibleObj<element_impl>
::request_visibility_recursive(IN_THREAD_ONLY, bool flag)
{
	this->request_child_visibility_recursive(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END

#endif
