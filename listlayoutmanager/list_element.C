/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_element_impl.H"

LIBCXXW_NAMESPACE_START

list_elementObj::list_elementObj(const ref<implObj> &impl)
	: elementObj(impl),
	impl(impl)
{
}

list_elementObj::~list_elementObj()=default;

focusable_impl list_elementObj::get_impl() const
{
	return impl;
}

///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
