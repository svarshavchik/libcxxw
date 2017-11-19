/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusable.H"
#include "child_element.H"
#include "generic_window_handler.H"
#include "run_as.H"
#include "xid_t.H"
#include "messages.H"
#include "batch_queue.H"
#include <X11/keysym.h>

#include <x/logger.H>

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::focusable, focusable_log);

focusableObj::focusableObj()
{
}

focusableObj::~focusableObj()=default;

bool next_key_pressed(const key_event &ke)
{
	return ke.keypress && next_key(ke);
}

bool next_key(const key_event &ke)
{
	return ke.notspecial() && ke.unicode == '\t';
}

bool prev_key_pressed(const key_event &ke)
{
	return ke.keypress && prev_key(ke);
}

bool prev_key(const key_event &ke)
{
	return ke.notspecial() && ke.keysym == XK_ISO_Left_Tab;
}

bool next_page_key_pressed(const key_event &ke)
{
	return ke.keypress && next_page_key(ke);
}

bool next_page_key(const key_event &ke)
{
	return ke.notspecial() &&
		(ke.keysym == XK_Page_Down || ke.keysym == XK_KP_Page_Down);
}

bool prev_page_key_pressed(const key_event &ke)
{
	return ke.keypress && prev_page_key(ke);
}

bool prev_page_key(const key_event &ke)
{
	return ke.notspecial() &&
		(ke.keysym == XK_Page_Up || ke.keysym == XK_KP_Page_Up);
}

bool select_key_pressed(const key_event &ke)
{
	return ke.keypress && select_key(ke);
}

bool select_key(const key_event &ke)
{
	if (ke.notspecial())
	{
		if (ke.unicode == '\n' || ke.unicode == ' ')
			return true;
	}
	return false;
}

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

	get_impl()->get_focusable_element().THREAD
		->run_as([=]
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
		([me=focusable(this), other]
		 (IN_THREAD_ONLY)
		 {
			 get_focus_before_in_thread(IN_THREAD, me, other);
		 });
}

void get_focus_before_in_thread(IN_THREAD_ONLY,	const focusable &me,
				const focusable &other)
{
	auto b=other->get_impl(0);

	auto n=me->internal_impl_count();

	while (n)
	{
		auto i=me->get_impl(--n);

		i->get_focus_before(IN_THREAD, b);
		b=i;
	}
}

void focusableObj::get_focus_after(const focusable &other)
{
	auto impl1=get_impl();
	auto impl2=other->get_impl();

	sanity_check(impl1, impl2)->run_as
		([me=focusable(this), other]
		 (IN_THREAD_ONLY)
		 {
			 get_focus_after_in_thread(IN_THREAD, me, other);
		 });
}

void get_focus_after_in_thread(IN_THREAD_ONLY, const focusable &me,
			       const focusable &other)
{
	get_focus_impl_after_in_thread
		(IN_THREAD,
		 me,
		 other->get_impl(other->internal_impl_count()-1));
}

void get_focus_impl_after_in_thread(IN_THREAD_ONLY, const focusable &me,
				    ref<focusableImplObj> a)
{
	auto n=me->internal_impl_count();
	for (size_t i=0; i<n; ++i)
	{
		auto impl=me->get_impl(i);

		impl->get_focus_after(IN_THREAD, a);
		a=impl;
	}
}

void focusableObj::get_focus_first()
{
	get_impl()->get_focusable_element().THREAD
		->run_as([me=focusable(this)]
			 (IN_THREAD_ONLY)
			 {
				 me->get_impl()->get_focusable_element()
					 .get_focus_first(IN_THREAD, me);
			 });
}

void focusableObj::on_keyboard_focus(const std::function<focus_callback_t> &cb)
{
	get_impl()->get_focusable_element().on_keyboard_focus(cb);
}

void focusableObj::on_key_event(const std::function<key_event_callback_t> &cb)
{
	get_impl()->get_focusable_element().on_key_event(cb);
}

void focusableObj::request_focus()
{
	auto impl=get_impl();

	// If request_focus() is invoked after using the layout manager to
	// change something in the container, the request_focus() action
	// may depend on the results of the layout manager changes. Therefore
	// we must schedule this to be batch-executed, after the batch job
	// finishes.

	impl->get_focusable_element().THREAD
		->get_batch_queue()
		->run_as([impl]
			 (IN_THREAD_ONLY)
			 {
				 LOG_FUNC_SCOPE(focusable_log);

				 if (!impl->get_focusable_element()
				     .enabled(IN_THREAD))
				 {
					 LOG_ERROR("Cannot set focus to requested display element");
					 return;
				 }

				 impl->set_focus_and_ensure_visibility
					 (IN_THREAD);
			 });
}

void focusableObj::autofocus(bool flag)
{
	get_impl()->autofocus=flag;
}
LIBCXXW_NAMESPACE_END
