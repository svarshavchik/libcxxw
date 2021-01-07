/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_always_visible_H
#define x_w_impl_always_visible_H

#include <x/w/impl/always_visiblefwd.H>

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl, bool initially_visible>
template<typename ...Args>
always_visibleObj<element_impl, initially_visible>
::always_visibleObj(Args && ...args)
	: element_impl{std::forward<Args>(args)...}
{
	if constexpr(initially_visible)
		{
			this->set_always_visible();
		}
}

//! Destructor

template<typename element_impl, bool initially_visible>
always_visibleObj<element_impl, initially_visible>
::~always_visibleObj()=default;

//! Override request_visibility_recursive()

//! Forward to request_child_visibility_recursive().

template<typename element_impl, bool initially_visible>
void always_visibleObj<element_impl, initially_visible>
::request_visibility_recursive(ONLY IN_THREAD, bool flag)
{
	this->request_child_visibility_recursive(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END

#endif
