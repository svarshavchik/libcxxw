/*
** Copyright 2017 Double Precision, Inc.
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


ref<obj> busy_impl::get_mcguffin() const
{
	return w->get_busy_mcguffin();
}

busy::busy()=default;

busy::~busy()=default;

LIBCXXW_NAMESPACE_END
