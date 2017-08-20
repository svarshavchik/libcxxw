/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "focus/focusable.H"
#include "generic_window_handler.H"
#include "child_element.H"
#include "messages.H"
#include "connection_thread.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

focusableImplObj::focusableImplObj()
	: in_focusable_fields_thread_only(false),
	  my_labels_thread_only(my_labels_t::create())
{
}

focusableImplObj::~focusableImplObj()=default;

bool focusableImplObj::enabled(IN_THREAD_ONLY)
{
	return get_focusable_element().enabled(IN_THREAD);
}

#define GET_FOCUSABLE_FIELD_ITER() ({		\
						\
	if (!in_focusable_fields(IN_THREAD))	\
		return;				\
						\
	focusable_fields_iter(IN_THREAD); })

void focusableImplObj::focusable_initialize(IN_THREAD_ONLY)
{
	if (in_focusable_fields(IN_THREAD))
		throw EXCEPTION("Internal element: multiply-linked focusable: "
				<< objname());

	auto &e=get_focusable_element();

	focusable_fields_iter(IN_THREAD)=
		focusable_fields(IN_THREAD).insert
		(e.initial_focusable_fields_insert_pos(IN_THREAD),
		 ref<focusableImplObj>(this));

	in_focusable_fields(IN_THREAD)=true;

	e.focusable_initialized(IN_THREAD, *this);
}

focusable_fields_t::iterator
child_elementObj::initial_focusable_fields_insert_pos(IN_THREAD_ONLY)
{
	return get_window_handler()
		.focusable_fields(IN_THREAD).end();
}

void focusableImplObj::focusable_deinitialize(IN_THREAD_ONLY)
{
	if (!in_focusable_fields(IN_THREAD))
		throw EXCEPTION("Internal element: multiply-linked focusable: "
				<< objname());

	// Remove all of my labels, redrawing them if needed.

	for (const auto &labels: *my_labels(IN_THREAD))
	{
		auto label_for=labels.getptr();

		if (!label_for)
			continue;

		label_for->with_link
			(IN_THREAD, [&]
			 (const auto &label_element,
			  const auto &thats_me)
			 {
				 auto before=label_element
					 ->draw_to_window_picture_as_disabled
					 (IN_THREAD);

				 label_element->data(IN_THREAD).label_for=
					 nullptr;

				 auto after=label_element
					 ->draw_to_window_picture_as_disabled
					 (IN_THREAD);

				 if (before == after)
					 return;

				 label_element->schedule_redraw(IN_THREAD);
			 });
	}


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

	if (window_handler.most_recent_keyboard_focus(IN_THREAD) == ptr_impl)
		// Find another field to switch the input
		// focus to. This will also null out
		// keyboard_focus, in every case. Either it
		// will find another focusable field, and
		// update keyboard_focus, or call remove_focus()
		// and null it out.
		next_focus(IN_THREAD, next_iter);
}

focusable_fields_t &focusableImplObj::focusable_fields(IN_THREAD_ONLY)
{
	return get_focusable_element().get_window_handler()
		.focusable_fields(IN_THREAD);
}

void focusableImplObj::set_enabled(IN_THREAD_ONLY, bool flag)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	auto &fe=get_focusable_element();

	if (fe.data(IN_THREAD).enabled == flag)
		return;

	fe.data(IN_THREAD).enabled=flag;
	// Because this may be the parent element, we want to recursively
	// redraw this.
	fe.schedule_redraw_recursively(IN_THREAD);

	// Redraw all my labels too.

	for (const auto &labels: *my_labels(IN_THREAD))
	{
		auto label_for=labels.getptr();

		if (!label_for)
			continue;

		label_for->with_link
			(IN_THREAD, [&]
			 (const auto &label_element,
			  const auto &thats_me)
			 {
				 label_element->schedule_redraw_recursively(IN_THREAD);
			 });
	}

	if (!flag)
		unfocus(IN_THREAD);
}

void focusableImplObj::unfocus(IN_THREAD_ONLY)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	auto ptr_impl=ptr<focusableImplObj>(this);

	if (get_focusable_element().get_window_handler()
	    .most_recent_keyboard_focus(IN_THREAD) != ptr_impl)
		return;

	next_focus(IN_THREAD);
}

bool elementObj::implObj
::draw_to_window_picture_as_disabled(IN_THREAD_ONLY)
{
	if (data(IN_THREAD).label_for)
	{
		// This element is a label for another focusable element.

		// Check if the other focusable element is disabled.

		bool focusable_enabled=true;

		data(IN_THREAD).label_for->with_link
			(IN_THREAD, [&]
			 (const auto &thats_me,
			  const auto &focusable)
			 {
				 focusable_enabled=
					 focusable->enabled(IN_THREAD);
			 });

		if (!focusable_enabled)
			return true;

	}
	return !enabled(IN_THREAD);
}

void elementObj::label_for(const focusable &f)
{
	auto check1=ref<generic_windowObj::handlerObj>(&impl->get_window_handler());

	auto check2=ref<generic_windowObj::handlerObj>(&f->get_impl()
						       ->get_focusable_element()
						       .get_window_handler());

	if (check1 != check2)
		throw EXCEPTION(_("Attempt to create a label for another window's element."));


	impl->get_window_handler().thread()->run_as
		([me=element(this), f]
		 (IN_THREAD_ONLY)
		 {
			 auto focusable_impl=f->get_impl();

			 auto before=me->impl
				 ->draw_to_window_picture_as_disabled
				 (IN_THREAD);

			 auto link=label_for::create(me->impl,
						     focusable_impl);

			 focusable_impl->my_labels(IN_THREAD)->push_back(link);

			 me->impl->data(IN_THREAD).label_for=link;

			 auto after=
				 me->impl->draw_to_window_picture_as_disabled
				 (IN_THREAD);

			 if (before == after)
				 return;

			 me->impl->schedule_redraw(IN_THREAD);
		 });
}

bool elementObj::implObj::process_button_event(IN_THREAD_ONLY,
					       const button_event &be,
					       xcb_timestamp_t timestamp)
{
	bool processed=false;

	if (data(IN_THREAD).label_for && !be.redirected)
	{
		data(IN_THREAD).label_for->with_link
			(IN_THREAD, [&, this]
			 (const auto &thats_me,
			  const auto &focusable)
			 {
				 elementObj::implObj &fe=
					 focusable->get_focusable_element();

				 // We need to re-report the last pointer
				 // motion event in the focusable element.

				 auto iamhere=
					 this->get_absolute_location(IN_THREAD);
				 auto itsoverthere=
					 fe.get_absolute_location(IN_THREAD);

				 auto updated_event=be;
				 updated_event.redirected=true;

				 auto x=this->data(IN_THREAD).last_motion_x;
				 auto y=this->data(IN_THREAD).last_motion_y;

				 x=coord_t::truncate(x+iamhere.x);
				 y=coord_t::truncate(y+iamhere.y);
				 x=coord_t::truncate(x-itsoverthere.x);
				 y=coord_t::truncate(y-itsoverthere.y);

				 motion_event me{be,
						 motion_event_type
						 ::button_event,
						 x, y};

				 fe.report_motion_event(IN_THREAD, me);

				 processed=fe
					 .process_button_event(IN_THREAD,
							       updated_event,
							       timestamp);
			 });
	}

	return processed;
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
		if ((*starting_iter)->enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *starting_iter);
			return;
		}
		++starting_iter;
	}

	for (starting_iter=ff.begin(); starting_iter != ff.end();
	     ++starting_iter)
		if ((*starting_iter)->enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *starting_iter);
			return;
		}

	get_focusable_element().get_window_handler()
		.unset_keyboard_focus(IN_THREAD);
}

void focusableImplObj::prev_focus(IN_THREAD_ONLY)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	while (iter != ff.begin())
	{
		--iter;
		if ((*iter)->enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *iter);
			return;
		}
	}

	for (iter=ff.end(); iter != ff.begin(); )
	{
		--iter;
		if ((*iter)->enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *iter);
			return;
		}
	}

	get_focusable_element().get_window_handler()
		.unset_keyboard_focus(IN_THREAD);
}

void focusableImplObj::set_focus_only(IN_THREAD_ONLY)
{
	if (!in_focusable_fields(IN_THREAD))
		throw EXCEPTION("Internal error: attempt to set focus to uninitialized"
				<< objname());

	get_focusable_element().get_window_handler()
		.set_keyboard_focus_to(IN_THREAD, focusable_impl(this));
}

void focusableImplObj::set_focus_and_ensure_visibility(IN_THREAD_ONLY)
{
	set_focus_only(IN_THREAD);
	get_focusable_element().ensure_entire_visibility(IN_THREAD);
}

void focusableImplObj::switch_focus(IN_THREAD_ONLY,
				    const focusable_impl &focus_to)
{
	focus_to->set_focus_and_ensure_visibility(IN_THREAD);

}

// Need to do a song-and-dance routine to make sure that both focusables
// get a clean bill of health from GET_FOCUSABLE_FIELD_ITER().

void focusableImplObj::get_focus_before(IN_THREAD_ONLY,
					const ref<focusableImplObj> &other)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	other->i_will_get_focus_after(IN_THREAD, *this);
}

void focusableImplObj::get_focus_after(IN_THREAD_ONLY,
				       const ref<focusableImplObj> &other)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	other->i_will_get_focus_before(IN_THREAD, *this);
}

void focusableImplObj::i_will_get_focus_before(IN_THREAD_ONLY,
					       focusableImplObj &other)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	auto new_iter=ff.insert(++iter, ref<focusableImplObj>(&other));

	ff.erase(other.focusable_fields_iter(IN_THREAD));
	other.focusable_fields_iter(IN_THREAD)=new_iter;
}

void focusableImplObj::i_will_get_focus_after(IN_THREAD_ONLY,
					      focusableImplObj &other)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	auto new_iter=ff.insert(iter, ref<focusableImplObj>(&other));

	ff.erase(other.focusable_fields_iter(IN_THREAD));
	other.focusable_fields_iter(IN_THREAD)=new_iter;
}

void focusableImplObj::creating_focusable_child_element()
{
	throw EXCEPTION(_("Focusable display elements cannot be created inside another focusable element."));
}

LIBCXXW_NAMESPACE_END
