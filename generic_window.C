/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "x/w/screen.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::generic_windowObj);

LIBCXXW_NAMESPACE_START

generic_windowObj::generic_windowObj(const ref<implObj> &impl)
	: elementObj(impl),
	  impl(impl)
{
}

generic_windowObj::~generic_windowObj() noexcept=default;

screen generic_windowObj::get_screen()
{
	return impl->get_screen();
}

const_screen generic_windowObj::get_screen() const
{
	return impl->get_screen();
}

LIBCXXW_NAMESPACE_END
