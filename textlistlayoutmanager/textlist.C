/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlist_impl.H"

LIBCXXW_NAMESPACE_START

textlistObj::textlistObj(const ref<implObj> &impl)
	: elementObj(impl),
	impl(impl)
{
}

textlistObj::~textlistObj()=default;

ref<focusableImplObj> textlistObj::get_impl() const
{
	return impl;
}

///////////////////////////////////////////////////////////////////////////


LIBCXXW_NAMESPACE_END
