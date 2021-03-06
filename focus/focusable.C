/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/child_element.H"
#include "generic_window_handler.H"
#include "run_as.H"
#include "xid_t.H"
#include "messages.H"
#include "batch_queue.H"
#include <X11/keysym.h>

#include <x/logger.H>

LIBCXXW_NAMESPACE_START

focusableObj::focusableObj()
{
}

focusableObj::~focusableObj()=default;

bool next_key_pressed(const key_event &ke)
{
	return ke.keypress && is_next_key(ke);
}

bool is_next_key(const key_event &ke)
{
	return ke.notspecial() && ke.unicode == '\t';
}

bool prev_key_pressed(const key_event &ke)
{
	return ke.keypress && is_prev_key(ke);
}

bool is_prev_key(const key_event &ke)
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

//! The default implementation uses get_impl() and returns it.

void focusableObj::do_get_impl(const function<internal_focusable_cb> &cb)
	const
{
	process_focusable_impls(cb, get_impl());
}

void focusableObj::set_enabled(bool flag)
{
	auto me=ref<focusableObj>(this);

	get_impl()->get_focusable_element().THREAD->run_as
		([me, flag]
		 (ONLY IN_THREAD)
		 {
			 me->set_enabled(IN_THREAD, flag);
		 });
}

void focusableObj::set_enabled(ONLY IN_THREAD, bool flag)
{
	// Enable or disable all real focusables

	get_impl([&]
		 (const auto &info)
		 {
			 for (size_t i=0; i<info.internal_impl_count; ++i)
				 info.impls[i]->set_enabled(IN_THREAD, flag);
		 });
}

static auto sanity_check(const focusable_impl &impl1,
			 const focusable_impl &impl2)
{
	auto h1=ref<generic_windowObj::handlerObj>
		(&impl1->get_focusable_element().get_window_handler());
	auto h2=ref<generic_windowObj::handlerObj>
		(&impl2->get_focusable_element().get_window_handler());

	if (h1 != h2)
		throw EXCEPTION(_("Cannot specify focus order between elements from different top level windows"));

	return h1->thread();
}

static auto sanity_check(const focusable_impl &impl1,
			 const std::vector<focusable> &focusables)
{
	auto h1=ref<generic_windowObj::handlerObj>
		(&impl1->get_focusable_element().get_window_handler());

	for (const auto &f:focusables)
	{
		auto h2=ref<generic_windowObj::handlerObj>
			(&f->get_impl()->get_focusable_element()
			 .get_window_handler());

		if (h1 != h2)
			throw EXCEPTION(_("Cannot specify focus order between elements from different top level windows"));
	}

	return h1->thread();
}

//! Retrieve internal_focusable_groups from two focusables.

//! Some implementations of do_get_impl() employ some kind of locking, so
//! we want to make sure we will always use the same locking order when
//! locking two elements.

template<typename callback>
static void get_two_impls(const focusable &focus1, const focusable &focus2,
			  callback &&cb);

static void do_get_two_impl(const focusable &focus1, const focusable &focus2,
			    const function<void
			    (const internal_focusable_group &,
			     const internal_focusable_group &)>
			    &cb);

template<typename callback>
inline static void get_two_impls(const focusable &focus1,
				 const focusable &focus2,
				 callback &&cb)
{
	do_get_two_impl(focus1, focus2,
			make_function<void (const internal_focusable_group &,
					    const internal_focusable_group &)>
			(std::forward<callback>(cb)));
}

static void do_get_two_impl(const focusable &focus1,
			    const focusable &focus2,
			    const function<void
			    (const internal_focusable_group &,
			     const internal_focusable_group &)>
			    &cb)
{
	if (focus1 < focus2)
	{
		focus1->get_impl
			([&]
			 (const auto &group1)
			 {
				 focus2->get_impl
					 ([&]
					  (const auto &group2)
					  {
						  cb(group1, group2);
					  });
			 });
		return;
	}

	focus2->get_impl
		([&]
		 (const auto &group2)
		 {
			 focus1->get_impl
				 ([&]
				  (const auto &group1)
				  {
					  cb(group1, group2);
				  });
		 });
}

void focusableObj::get_focus_before(const focusable &other)
{
	auto impl1=get_impl();
	auto impl2=other->get_impl();

	sanity_check(impl1, impl2)->run_as
		([me=focusable(this), other]
		 (ONLY IN_THREAD)
		 {
			 get_focus_before_in_thread(IN_THREAD, me, other);
		 });
}

void focusableObj::get_focus_before(ONLY IN_THREAD,
				    const focusable &other)
{
	sanity_check(get_impl(), other->get_impl());

	get_focus_before_in_thread(IN_THREAD, ref{this}, other);
}

void get_focus_before_in_thread(ONLY IN_THREAD,	const focusable &me,
				const focusable &other)
{
	get_two_impls
		(me, other,
		 [&]
		 (const auto &me_group,
		  const auto &other_group)
		 {
			 // Start with other's first internal focusable.

			 auto b=other_group.impls[0];

			 // Put my last focusable before 'b', then set 'b'
			 // to this focusable. Lather, rinse, repeat.

			 auto n=me_group.internal_impl_count;

			 while (n)
			 {
				 auto i=me_group.impls[--n];

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
		([me=focusable(this), other]
		 (ONLY IN_THREAD)
		 {
			 get_focus_after_in_thread(IN_THREAD, me, other);
		 });
}

void focusableObj::get_focus_after(ONLY IN_THREAD,
				   const focusable &other)
{
	sanity_check(get_impl(), other->get_impl());

	get_focus_after_in_thread(IN_THREAD, ref{this}, other);
}

// Put me after 'other' in focus order.

// Take other's last focusable in its focusable group, then iterate over my
// focusable group, putting each individual focusable after that one, then
// make "that one" the one that I just ordered.

static void get_focus_impl_after_in_thread_with_group
(ONLY IN_THREAD,
 const internal_focusable_group &me_group,
 focusable_impl a)
{
	auto n=me_group.internal_impl_count;

	for (size_t i=0; i<n; ++i)
	{
		auto impl=me_group.impls[i];

		impl->get_focus_after(IN_THREAD, a);
		a=impl;
	}
}

void get_focus_after_in_thread(ONLY IN_THREAD, const focusable &me,
			       const focusable &other)
{
	get_two_impls
		(me, other,
		 [&]
		 (const auto &me_group,
		  const auto &other_group)
		 {
			 get_focus_impl_after_in_thread_with_group
				 (IN_THREAD,
				  me_group,
				  other_group.impls
				  [other_group.internal_impl_count-1]);
		 });
}

void get_focus_impl_after_in_thread(ONLY IN_THREAD, const focusable &me,
				    const focusable_impl &a)
{
	me->get_impl
		([&]
		 (const auto &me_group)
		 {
			 get_focus_impl_after_in_thread_with_group
				 (IN_THREAD,
				  me_group, a);
		 });
}

void focusableObj::get_focus_first()
{
	get_impl()->get_focusable_element().THREAD
		->run_as([me=focusable(this)]
			 (ONLY IN_THREAD)
			 {
				 me->get_focus_first(IN_THREAD);
			 });
}

void focusableObj::get_focus_first(ONLY IN_THREAD)
{
	auto me=ref{this};

	me->get_impl()->get_focusable_element().get_focus_first(IN_THREAD, me);
}

void focusableObj::get_focus_after_me(const std::vector<focusable> &others)
{
	sanity_check(get_impl(), others)->run_as
		([me=focusable(this), others]
		 (ONLY IN_THREAD)
		 {
			 me->get_focus_after_me(IN_THREAD, others);
		 });
}

void focusableObj::get_focus_after_me(ONLY IN_THREAD,
				      const std::vector<focusable> &others)
{
	sanity_check(get_impl(), others);

	auto me=ref{this};

	for (const auto &f:others)
	{
		get_focus_after_in_thread(IN_THREAD, f, me);
		me=f;
	}
}

void focusableObj::get_focus_before_me(const std::vector<focusable> &others)
{
	sanity_check(get_impl(), others)->run_as
		([me=focusable(this), others]
		 (ONLY IN_THREAD)
		 {
			 me->get_focus_before_me(IN_THREAD, others);
		 });
}

void focusableObj::get_focus_before_me(ONLY IN_THREAD,
				       const std::vector<focusable> &others)
{
	sanity_check(get_impl(), others);

	auto me=ref{this};

	for (const auto &f:others)
		get_focus_before_in_thread(IN_THREAD, f, me);
}

void focusableObj::on_keyboard_focus(const functionref<focus_callback_t> &cb)
{
	get_impl()->get_focusable_element().on_keyboard_focus(cb);
}

void focusableObj::on_key_event(const functionref<key_event_callback_t> &cb)
{
	get_impl()->get_focusable_element().on_key_event(cb);
}

void focusableObj::request_focus(bool now_or_never)
{
	get_impl()->get_focusable_element().THREAD
		->run_as
		([impl=get_impl(), now_or_never]
		 (ONLY IN_THREAD)
		 {
			 impl->request_focus_if_possible(IN_THREAD,
							 now_or_never);
		 });
}

void focusableObj::request_focus(ONLY IN_THREAD, bool now_or_never)
{
	get_impl()->request_focus_if_possible(IN_THREAD,
					      now_or_never);
}

void focusableObj::autofocus(bool flag)
{
	get_impl()->autofocus=flag;
}

void process_focusable_impls_from_focusables
(const function<internal_focusable_cb> &cb,
 const std::vector<focusable> &v)
{
	std::vector<focusable_impl> i;

	for (const auto &f:v)
	{
		f->get_impl
			([&]
			 (const auto &internal_group)
			 {
				 i.reserve(i.size()
					   +internal_group.internal_impl_count);
				 i.insert(i.end(),
					  internal_group.impls,
					  internal_group.impls
					  +internal_group.internal_impl_count);
			 });
	}

	internal_focusable_group g{i.size(), &i.at(0)};
	cb(g);
}

LIBCXXW_NAMESPACE_END
