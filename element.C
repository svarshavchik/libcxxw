/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"
#include "element_screen.H"

LIBCXXW_NAMESPACE_START

elementObj::elementObj(const ref<implObj> &impl)
	: impl(impl)
{
}

elementObj::~elementObj()
{
}

screen elementObj::get_screen()
{
       return impl->get_screen();
}

const_screen elementObj::get_screen() const
{
       return impl->get_screen();
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
