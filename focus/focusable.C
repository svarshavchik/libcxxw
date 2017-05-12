/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusable.H"
#include "child_element.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

focusableObj::focusableObj()
{
}

focusableObj::~focusableObj()=default;

void focusableObj::set_enabled(bool flag)
{
	auto me=ref<focusableObj>(this);

	auto impl=get_impl();

	impl->get_focusable_element().get_window_handler()
		.thread()->run_as(RUN_AS,
				[=]
				(IN_THREAD_ONLY)
				{
					me->get_impl()->set_enabled(IN_THREAD,
								    flag);
				});
}

LIBCXXW_NAMESPACE_END
