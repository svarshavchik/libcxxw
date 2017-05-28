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

size_t focusableObj::internal_impl_count() const
{
	return 1;
}

ref<focusableImplObj> focusableObj::get_impl(size_t) const
{
	return get_impl();
}

void focusableObj::set_enabled(bool flag)
{
	auto me=ref<focusableObj>(this);

	get_impl()->get_focusable_element().get_window_handler()
		.thread()->run_as(RUN_AS,
				[=]
				(IN_THREAD_ONLY)
				{
					// Enable or disable all real focusables

					auto n=me->internal_impl_count();

					for (size_t i=0; i<n; ++i)
						me->get_impl(i)
							->set_enabled(IN_THREAD,
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
		 [me=focusable(this), other]
		 (IN_THREAD_ONLY)
		 {
			 auto b=other->get_impl(0);

			 auto n=me->internal_impl_count();

			 while (n)
			 {
				 auto i=me->get_impl(--n);

				 i->get_focus_before(IN_THREAD, b);
				 b=i;
			 }
		 });
}

void focusableObj::get_focus_after(const focusable &other)
{
	auto impl1=get_impl();
	auto impl2=other->get_impl();

	sanity_check(impl1, impl2)->run_as
		(RUN_AS,
		 [me=focusable(this), other]
		 (IN_THREAD_ONLY)
		 {
			 auto a=other->get_impl(other->internal_impl_count()-1);

			 auto n=me->internal_impl_count();
			 for (size_t i=0; i<n; ++i)
			 {
				 auto impl=me->get_impl(i);

				 impl->get_focus_after(IN_THREAD, a);
				 a=impl;
			 }
		 });
}

LIBCXXW_NAMESPACE_END
