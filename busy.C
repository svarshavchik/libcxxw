/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "busy.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

busy_impl::busy_impl(elementObj::implObj &i,
		     const connection_thread &thread)
	: busy_impl(ref<generic_windowObj::handlerObj>(&i.get_window_handler()),
		    thread)
{
}

busy_impl::busy_impl(const ref<generic_windowObj::handlerObj> &w,
		     const connection_thread &thread) : busy(thread), w(w)
{
}

busy_impl::~busy_impl()=default;


ref<obj> busy_impl::get_mcguffin() const
{
	return w->get_busy_mcguffin();
}

busy::busy(const connection_thread &thread) : thread(thread)
{
}

busy::~busy()=default;

LIBCXXW_NAMESPACE_END
