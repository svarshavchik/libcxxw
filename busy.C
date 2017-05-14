/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "busy.H"

LIBCXXW_NAMESPACE_START

busy_impl::busy_impl(elementObj::implObj &i)
	: busy_impl(ref<generic_windowObj::handlerObj>(&i.get_window_handler()))
{
}

busy_impl::busy_impl(const ref<generic_windowObj::handlerObj> &w) : w(w)
{
}

busy_impl::~busy_impl()=default;


x::ref<x::obj> busy_impl::get_mcguffin() const
{
	generic_windowObj::handlerObj::busy_mcguffin_t::lock
		lock{w->busy_mcguffin};

	auto p=lock->getptr();

	if (p) return p;

	auto n=ref<obj>::create();

	*lock=n;
	return n;
}

busy::busy()=default;

busy::~busy()=default;

LIBCXXW_NAMESPACE_END
