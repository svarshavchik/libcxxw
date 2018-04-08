/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/main_window.H"
#include "focus/focusable.H"
#include "focus/label_for.H"
#include "generic_window_handler.H"
#include "x/w/impl/child_element.H"
#include "messages.H"
#include "busy.H"
#include "connection_thread.H"
#include "catch_exceptions.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

focusableObj::implObj::implObj()
	: in_focusable_fields_thread_only(false),
	  my_labels_thread_only(my_labels_t::create()),
	  autofocus(false)
{
}

focusableObj::implObj::~implObj()=default;

bool focusableObj::implObj::focusable_enabled(ONLY IN_THREAD)
{
	return get_focusable_element().enabled(IN_THREAD);
}

#define GET_FOCUSABLE_FIELD_ITER() ({		\
						\
	if (!in_focusable_fields(IN_THREAD))	\
		return;				\
						\
	focusable_fields_iter(IN_THREAD); })

void focusableObj::implObj::focusable_initialize(ONLY IN_THREAD)
{
	if (in_focusable_fields(IN_THREAD))
		throw EXCEPTION("Internal element: multiply-linked focusable: "
				<< objname());

	auto &e=get_focusable_element();

	auto &ff=focusable_fields(IN_THREAD);

	focusable_fields_iter(IN_THREAD)=ff.insert(ff.end(),
						   ref<focusableObj::implObj>(this));

	in_focusable_fields(IN_THREAD)=true;

	e.focusable_initialized(IN_THREAD, *this);
}

void focusableObj::implObj::focusable_deinitialize(ONLY IN_THREAD)
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

				 label_element->schedule_redraw_recursively
					 (IN_THREAD);
			 });
	}


	auto &window_handler=get_focusable_element().get_window_handler();

	auto &ff=window_handler.focusable_fields(IN_THREAD);

	auto ptr_impl=ptr<focusableObj::implObj>(this);

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
		next_focus(IN_THREAD, next_iter, {});
}

focusable_fields_t &focusableObj::implObj::focusable_fields(ONLY IN_THREAD)
{
	return get_focusable_element().get_window_handler()
		.focusable_fields(IN_THREAD);
}

void focusableObj::implObj::set_enabled(ONLY IN_THREAD, bool flag)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	auto &fe=get_focusable_element();

	if (fe.data(IN_THREAD).enabled == flag)
		return;

	fe.data(IN_THREAD).enabled=flag;
	// Because this may be the parent element, we want to recursively
	// redraw this.
	fe.enablability_changed(IN_THREAD);

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

void focusableObj::implObj::unfocus_later(ONLY IN_THREAD)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	auto ptr_impl=ptr<focusableObj::implObj>(this);

	if (get_focusable_element().get_window_handler()
	    .most_recent_keyboard_focus(IN_THREAD) != ptr_impl)
		return;

	IN_THREAD->idle_callbacks(IN_THREAD)
		->push_back([me=ref(this)]
			    (ONLY IN_THREAD)
			    {
				    me->unfocus(IN_THREAD);
			    });
}

void focusableObj::implObj::unfocus(ONLY IN_THREAD)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	auto ptr_impl=ptr<focusableObj::implObj>(this);

	if (get_focusable_element().get_window_handler()
	    .most_recent_keyboard_focus(IN_THREAD) != ptr_impl)
		return;

#ifdef TEST_UNFOCUS
	TEST_UNFOCUS();
#endif
	next_focus(IN_THREAD, {});
}

bool elementObj::implObj
::draw_to_window_picture_as_disabled(ONLY IN_THREAD)
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
					 focusable->focusable_enabled(IN_THREAD);
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
		 (ONLY IN_THREAD)
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

			 me->impl->schedule_redraw_recursively(IN_THREAD);
		 });
}

bool elementObj::implObj::enabled(ONLY IN_THREAD)
{
	// This element has been removed from the container.
	//
	// The destructor of the public object will make sure that
	// the focus has been properly removed from me. But, when an
	// entire container is removed, remove() recursively sets the removed
	// flag on the container's entire contents, and this is going to
	// prevent the input focus from bouncing until it escapes the
	// elements that are being destroyed.

	if (data(IN_THREAD).removed || !data(IN_THREAD).inherited_visibility)
		return false;

	// If this element is a label for another element we have to check
	// that element's enabled flag.
	auto check_this=ref(this);

	if (data(IN_THREAD).label_for)
	{
		data(IN_THREAD).label_for->with_link
			(IN_THREAD, [&]
			 (const auto &thats_me,
			  const auto &focusable)
			 {
				 check_this=ref(&focusable
						->get_focusable_element());
			 });
	}

	// Should check this again now, in case this is the labeled element.
	if (check_this->data(IN_THREAD).removed ||
	    !check_this->data(IN_THREAD).inherited_visibility)
		return false;

	// Finally.
	return check_this->data(IN_THREAD).enabled;
}

bool elementObj::implObj
::process_button_event_if_enabled(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp)
{
	if (!enabled(IN_THREAD))
		return false;

	return process_button_event(IN_THREAD, be, timestamp);
}

bool elementObj::implObj::process_button_event(ONLY IN_THREAD,
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

				 x=coord_t::truncate(coord_t::truncate(x)
						     -coord_t::truncate
						     (itsoverthere.x));
				 y=coord_t::truncate(coord_t::truncate(y)
						     -coord_t::truncate
						     (itsoverthere.y));

				 motion_event me{be,
						 activate_for(be)
						 ? motion_event_type
						 ::button_action_event
						 : motion_event_type
						 ::button_event,
						 x, y};

				 fe.report_motion_event(IN_THREAD, me);

				 processed=fe
					 .process_button_event(IN_THREAD,
							       updated_event,
							       timestamp);
			 });
	}

	if (!processed && activate_for(be) && be.button == 3 &&
	    data(IN_THREAD).contextpopup_callback)
	{
		auto main_window=get_window_handler().get_main_window();

		if (main_window)
		{
			busy_impl yes_i_am{*this};
			try {
				data(IN_THREAD).contextpopup_callback
					(IN_THREAD, &be, yes_i_am);
			} REPORT_EXCEPTIONS(main_window);
		}

		processed=true;
	}
	return processed;
}

void focusableObj::implObj::next_focus(ONLY IN_THREAD,
				  const callback_trigger_t &trigger)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	next_focus(IN_THREAD, ++iter, trigger);
}

void focusableObj::implObj::next_focus(ONLY IN_THREAD,
				  focusable_fields_t::iterator starting_iter,
				  const callback_trigger_t &trigger)
{
	auto &ff=focusable_fields(IN_THREAD);

	while (starting_iter != ff.end())
	{
		if ((*starting_iter)->focusable_enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *starting_iter, trigger);
			return;
		}
		++starting_iter;
	}

	for (starting_iter=ff.begin(); starting_iter != ff.end();
	     ++starting_iter)
		if ((*starting_iter)->focusable_enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *starting_iter, trigger);
			return;
		}

	get_focusable_element().get_window_handler()
		.unset_keyboard_focus(IN_THREAD, trigger);
}

void focusableObj::implObj::prev_focus(ONLY IN_THREAD,
				  const callback_trigger_t &trigger)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	while (iter != ff.begin())
	{
		--iter;
		if ((*iter)->focusable_enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *iter, trigger);
			return;
		}
	}

	for (iter=ff.end(); iter != ff.begin(); )
	{
		--iter;
		if ((*iter)->focusable_enabled(IN_THREAD))
		{
			switch_focus(IN_THREAD, *iter, trigger);
			return;
		}
	}

	get_focusable_element().get_window_handler()
		.unset_keyboard_focus(IN_THREAD, trigger);
}

void focusableObj::implObj::set_focus_only(ONLY IN_THREAD,
				      const callback_trigger_t &trigger)
{
	if (!in_focusable_fields(IN_THREAD))
		throw EXCEPTION("Internal error: attempt to set focus to uninitialized"
				<< objname());

	get_focusable_element().get_window_handler()
		.set_keyboard_focus_to(IN_THREAD, focusable_impl(this),
				       trigger);
}

void focusableObj::implObj
::set_focus_and_ensure_visibility(ONLY IN_THREAD,
				  const callback_trigger_t &trigger)
{
	set_focus_only(IN_THREAD, trigger);
	get_focusable_element().ensure_entire_visibility(IN_THREAD);
}

void focusableObj::implObj::switch_focus(ONLY IN_THREAD,
				    const focusable_impl &focus_to,
				    const callback_trigger_t &trigger)
{
	focus_to->set_focus_and_ensure_visibility(IN_THREAD, trigger);

}

bool focusableObj::implObj::ok_to_lose_focus(ONLY IN_THREAD,
					const callback_trigger_t &trigger)
{
	return true;
}

// Need to do a song-and-dance routine to make sure that both focusables
// get a clean bill of health from GET_FOCUSABLE_FIELD_ITER().

void focusableObj::implObj::get_focus_before(ONLY IN_THREAD,
					const ref<focusableObj::implObj> &other)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	other->i_will_get_focus_after(IN_THREAD, *this);
}

void focusableObj::implObj::get_focus_after(ONLY IN_THREAD,
				       const ref<focusableObj::implObj> &other)
{
	(void)GET_FOCUSABLE_FIELD_ITER();

	other->i_will_get_focus_before(IN_THREAD, *this);
}

void focusableObj::implObj::i_will_get_focus_before(ONLY IN_THREAD,
					       focusableObj::implObj &other)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	auto new_iter=ff.insert(++iter, ref<focusableObj::implObj>(&other));

	ff.erase(other.focusable_fields_iter(IN_THREAD));
	other.focusable_fields_iter(IN_THREAD)=new_iter;
}

void focusableObj::implObj::i_will_get_focus_after(ONLY IN_THREAD,
					      focusableObj::implObj &other)
{
	auto iter=GET_FOCUSABLE_FIELD_ITER();

	auto &ff=focusable_fields(IN_THREAD);

	auto new_iter=ff.insert(iter, ref<focusableObj::implObj>(&other));

	ff.erase(other.focusable_fields_iter(IN_THREAD));
	other.focusable_fields_iter(IN_THREAD)=new_iter;
}

void focusableObj::implObj::creating_focusable_child_element()
{
	throw EXCEPTION(_("Focusable display elements cannot be created inside another focusable element."));
}

LIBCXXW_NAMESPACE_END
