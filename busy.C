/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "busy.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

busy_impl::busy_impl(elementObj::implObj &i)
	: busy_impl(ref(&i.get_window_handler()))
{
}

busy_impl::busy_impl(const ref<generic_windowObj::handlerObj> &w) : w(w)
{
}

busy_impl::~busy_impl()=default;


ref<obj> busy_impl::get_shade_busy_mcguffin() const
{
	return w->get_shade_busy_mcguffin();
}

ref<obj> busy_impl::get_wait_busy_mcguffin() const
{
	return w->get_wait_busy_mcguffin();
}

busy::busy()=default;

busy::~busy()=default;

LIBCXXW_NAMESPACE_END
