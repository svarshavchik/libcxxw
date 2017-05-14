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
#include "messages.H"

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

static auto sanity_check(const auto &impl1,
			 const auto &impl2)
{
	auto h1=ref<generic_windowObj::handlerObj>
		(&impl1->get_focusable_element().get_window_handler());
	auto h2=ref<generic_windowObj::handlerObj>
		(&impl2->get_focusable_element().get_window_handler());

	if (h1 != h2)
		throw EXCEPTION(_("Cannot specify focus order between elements from different top level windows"));

	return h1->thread();
}

void focusableObj::get_focus_before(const focusable &other)
{
	auto impl1=get_impl();
	auto impl2=other->get_impl();

	sanity_check(impl1, impl2)->run_as
		(RUN_AS,
		 [=]
		 (IN_THREAD_ONLY)
		 {
			 impl1->get_focus_before(IN_THREAD, impl2);
		 });
}

void focusableObj::get_focus_after(const focusable &other)
{
	auto impl1=get_impl();
	auto impl2=other->get_impl();

	sanity_check(impl1, impl2)->run_as
		(RUN_AS,
		 [=]
		 (IN_THREAD_ONLY)
		 {
			 impl1->get_focus_after(IN_THREAD, impl2);
		 });
}

LIBCXXW_NAMESPACE_END
