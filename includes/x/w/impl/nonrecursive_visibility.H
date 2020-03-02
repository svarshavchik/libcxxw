/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_nonrecursive_visibility_H
#define x_w_impl_nonrecursive_visibility_H

#include <x/w/impl/nonrecursive_visibilityfwd.H>

LIBCXXW_NAMESPACE_START

template<typename container_impl>
nonrecursive_visibilityObj<container_impl>
::~nonrecursive_visibilityObj()=default;

template<typename container_impl>
void nonrecursive_visibilityObj<container_impl>
::request_visibility_recursive(ONLY IN_THREAD, bool flag)
{
	this->request_visibility(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END

#endif
