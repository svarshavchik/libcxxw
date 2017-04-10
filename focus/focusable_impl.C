/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable.H"
#include "focus/focusable.H"
#include "generic_window_handler.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

focusableImplObj::focusableImplObj(bool enabled)
	: enabled_flag_thread_only(enabled)
{
}

focusableImplObj::~focusableImplObj()=default;

bool focusableImplObj::is_enabled(IN_THREAD_ONLY)
{
	// This element has been removed from the container.
	//
	// The destructor of the public object will make sure that
	// the focus has been properly removed from me. But, when an
	// entire container is removed, remove() recursive sets the removed
	// flag on the container's entire contents, and this is going to
	// prevent the input focus from bouncing until it escapes the
	// elements that are being destroyed.

	const auto &data=get_focusable_element().data(IN_THREAD);

	if (data.removed || !data.inherited_visibility)
		return false;

	return enabled_flag(IN_THREAD);
}

#define GET_FOCUSABLE_FIELD_ITER() ({		\
						\
	if (!in_focusable_fields(IN_THREAD))	\
		return;				\
						\
	focusable_fields_iter(IN_THREAD); })

void focusableImplObj::focusable_initialize(IN_THREAD_ONLY)
{
	focusable_fields_iter(IN_THREAD)=
		focusable_fields(IN_THREAD).insert
		(get_focusable_element().initial_focusable_fields_insert_pos(IN_THREAD),
		 ref<focusableImplObj>(this));

	in_focusable_fields(IN_THREAD)=true;
}

focusable_fields_t::iterator
child_elementObj::initial_focusable_fields_insert_pos(IN_THREAD_ONLY)
{
	return get_window_handler()
		.focusable_fields(IN_THREAD).end();
}

void focusableImplObj::focusable_deinitialize(IN_THREAD_ONLY)
{
	auto &window_handler=get_focusable_element().get_window_handler();

	auto &ff=window_handler.focusable_fields(IN_THREAD);

	auto ptr_impl=ptr<focusableImplObj>(this);

	// Remove the focusable field from focusable_fields,
	// in every case.

	auto next_iter=ff.erase(focusable_fields_iter(IN_THREAD));
	in_focusable_fields(IN_THREAD)=false;

	// If this field is not currently receiving input
	// focus we're golden. Just removing it from
	// focusable_fields is enough.

	if (window_handler.current_focus(IN_THREAD) == ptr_impl)
		// Find another field to switch the input
		// focus to. This will also null out
		// current_focus, in every case. Either it
		// will find another focusable field, and
		// update current_focus, or call remove_focus()
		// and null it out.
		next_focus(IN_THREAD, next_iter);
}

focusable_fields_t &focusableImplObj::focusable_fields(IN_THREAD_ONLY)
{
	return get_focusable_element().get_window_handler()
		.focusable_fields(IN_THREAD);
}

void focusableImplObj::next_focus(IN_THREAD_ONLY)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	next_focus(IN_THREAD, ++iter);
}

void focusableImplObj::next_focus(IN_THREAD_ONLY,
				  focusable_fields_t::iterator starting_iter)
{
	auto &ff=focusable_fields(IN_THREAD);

	while (starting_iter != ff.end())
	{
		if ((*starting_iter)->is_enabled(IN_THREAD))
			break;
		++starting_iter;
	}

	if (starting_iter == ff.end())
		for (starting_iter=ff.begin(); starting_iter != ff.end();
		     ++starting_iter)
			if ((*starting_iter)->is_enabled(IN_THREAD))
				break;

	if (starting_iter == ff.end())
		remove_focus(IN_THREAD);
	else
		switch_focus(IN_THREAD, *starting_iter);
}

void focusableImplObj::prev_focus(IN_THREAD_ONLY)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	while (iter != ff.begin())
	{
		--iter;
		if ((*iter)->is_enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *iter);
			return;
		}
	}

	for (iter=ff.end(); iter != ff.begin(); )
	{
		--iter;
		if ((*iter)->is_enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *iter);
			return;
		}
	}

	remove_focus(IN_THREAD);
}

void focusableImplObj::remove_focus(IN_THREAD_ONLY)
{
	get_focusable_element().get_window_handler()
		.current_focus(IN_THREAD)=ptr<focusableImplObj>();

	// See element_focusable.C

	get_focusable_element().lose_focus(IN_THREAD,
					   &elementObj::implObj
					   ::report_keyboard_focus);
}

void focusableImplObj::set_focus(IN_THREAD_ONLY)
{
	auto &e=get_focusable_element();
	auto &h=e.get_window_handler();

	auto current_focus=h.current_focus(IN_THREAD);

	if (!current_focus.null())
	{
		current_focus->switch_focus(IN_THREAD, focusable_impl(this));
		return;
	}

	h.current_focus(IN_THREAD)=focusable_impl(this);
	e.request_focus(IN_THREAD,
			ptr<elementObj::implObj>(),
			&elementObj::implObj::report_keyboard_focus);
}

void focusableImplObj::switch_focus(IN_THREAD_ONLY,
				    const focusable_impl &focus_to)
{
	get_focusable_element().get_window_handler()
		.current_focus(IN_THREAD)=focus_to;

	focus_to->get_focusable_element()
		.request_focus(IN_THREAD,
			       ptr<elementObj::implObj>(&get_focusable_element()),
			       &elementObj::implObj::report_keyboard_focus);
}
LIBCXXW_NAMESPACE_END
