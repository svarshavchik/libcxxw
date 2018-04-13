/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_always_visible_element_H
#define x_w_impl_always_visible_element_H

#include <x/w/impl/always_visible_elementfwd.H>
#include <utility>

LIBCXXW_NAMESPACE_START

//! Constructor

template<typename element_impl>
template<typename ...Args>
always_visible_elementObj<element_impl>
::always_visible_elementObj(Args && ...args)
	: element_impl(std::forward<Args>(args)...)
{
	this->set_always_visible();
}

//! Destructor
template<typename element_impl>
always_visible_elementObj<element_impl>
::~always_visible_elementObj()=default;

//! Override request_visibility()

template<typename element_impl>
void always_visible_elementObj<element_impl>
::request_visibility(ONLY IN_THREAD, bool flag)
{
}

LIBCXXW_NAMESPACE_END

#endif
