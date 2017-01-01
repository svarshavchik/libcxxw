/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"

LIBCXXW_NAMESPACE_START

elementObj::elementObj(const ref<implObj> &impl)
	: impl(impl)
{
}

elementObj::~elementObj() noexcept
{
}

void elementObj::show()
{
	impl->request_visibility(true);
}

void elementObj::hide()
{
	impl->request_visibility(false);
}


LIBCXXW_NAMESPACE_END
