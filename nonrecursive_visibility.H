/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef nonrecursive_visibility_H
#define nonrecursive_visibility_H

#include "nonrecursive_visibilityfwd.H"

LIBCXXW_NAMESPACE_START

template<typename container_impl>
nonrecursive_visibilityObj<container_impl>
::~nonrecursive_visibilityObj()=default;

template<typename container_impl>
void nonrecursive_visibilityObj<container_impl>
::request_visibility_recursive(IN_THREAD_ONLY, bool flag)
{
	this->request_visibility(IN_THREAD, flag);
}

LIBCXXW_NAMESPACE_END

#endif
