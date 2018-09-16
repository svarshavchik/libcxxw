/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_thread.H"
#include "generic_window_handler.H"
#include "x/w/impl/element.H"
#include "x/w/impl/container.H"
#include "x/w/impl/layoutmanager.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

void connection_threadObj::insert_element_set(element_set_t &s,
					      const element_impl &i)
{
	s[i->nesting_level].insert(i);
}

// Remove the lowest element from the non-empty element_set_t.
//
// Returns the next element with the largest nesting_level. This is used
// for optimally processing visibility changes, for example, so that the
// so that the topmost element's visibility is updated last.

element_impl connection_threadObj::next_lowest_element(element_set_t &s)
{
	auto iter=--s.end();

	auto &last_set=iter->second;

	auto element_iter=last_set.begin();

	auto element=*element_iter;

	last_set.erase(element_iter);

	if (last_set.empty())
		s.erase(iter);

	return element;
}

// Remove the highest element from the non-empty element_set_t.
//
// Returns the next element with the smallest nesting_level. This is used
// for optimally processing sizing changes, for example, so that the
// so that the consequences of the topmost element's size change is processed
// last, since the topmost element may choose to recalculate the sizes of its
// child elements.

element_impl connection_threadObj::next_highest_element(element_set_t &s)
{
	auto iter=s.begin();

	auto &first_set=iter->second;

	auto element_iter=first_set.begin();

	auto element=*element_iter;

	first_set.erase(element_iter);

	if (first_set.empty())
		s.erase(iter);

	return element;
}

bool connection_threadObj::resize_pending(ONLY IN_THREAD,
					  const element_impl &e,
					  int &poll_for)
{
	auto &wh=e->get_window_handler();

	if (!wh.resizing(IN_THREAD))
		return false;

	auto now=tick_clock_t::now();

	if (now >= wh.resizing_timeout(IN_THREAD))
	{
		wh.resizing(IN_THREAD)=false;
		return false;
	}

	compute_poll_until(now, wh.resizing_timeout(IN_THREAD), poll_for);

	return true;
}

void connection_threadObj
::do_lowest_sizable_element_first(ONLY IN_THREAD,
				  element_set_t &s,
				  int &poll_for,
				  const function<void (const element_impl &)>
				  &f)
{
	// Start at the end of the nesting level map, work our way up.

	auto iter=s.end();

	while (iter != s.begin())
	{
		--iter;

		auto &last_set=iter->second;

		// Work our way through all elements at this nesting level.

		auto b=last_set.begin(), e=last_set.end();

		while (b != e)
		{
			auto element=*b;

			if (resize_pending(IN_THREAD, element, poll_for))
			{
				++b;
				continue;
			}

			b=last_set.erase(b);

			f(element);
		}

		if (last_set.empty())
			iter=s.erase(iter);
	}
}

void connection_threadObj
::do_highest_sizable_element_first(ONLY IN_THREAD,
				   element_set_t &s,
				   int &poll_for,
				   const function<void (const element_impl &)>
				   &f)
{
	// Start at the end of the nesting level map, work our way up.

	auto iter=s.begin();

	while (iter != s.end())
	{
		auto &first_set=iter->second;

		auto p=iter++;

		// Work our way through all elements at this nesting level.

		auto b=first_set.begin(), e=first_set.end();

		while (b != e)
		{
			auto element=*b;

			if (resize_pending(IN_THREAD, element, poll_for))
			{
				++b;
				continue;
			}

			b=first_set.erase(b);

			f(element);
		}

		if (first_set.empty())
			s.erase(p);
	}
}

// Some display element changed their visibility. Invoke
// their update_visibility() methods.

bool connection_threadObj::process_visibility_updated(ONLY IN_THREAD,
						      int &poll_for)
{
	if (visibility_updated_thread_only->empty())
		return false;

	while (!visibility_updated_thread_only->empty())
	{
		auto e=next_lowest_element(*visibility_updated_thread_only);

		LOG_TRACE("update_visibility: " << e->objname()
			  << "(" << &*e << ")");
		try {
			e->update_visibility(IN_THREAD);
		} CATCH_EXCEPTIONS;
	}
	return true;
}

bool connection_threadObj::recalculate_containers(ONLY IN_THREAD, int &poll_for)
{
	if (containers_2_recalculate_thread_only->empty())
		return false;

	// Get the next container to be recalculated, with the
	// highest nesting level.

	while (!containers_2_recalculate_thread_only->empty())
	{
		auto p=--containers_2_recalculate_thread_only->end();

		auto first=p->second.begin();

		auto container=*first;

		// Wait until recalculate() is invoked before removing it.

		// A layout manager might invoke theme_updated() for a
		// newly-added child element, which might result in the
		// child element attempting to schedule its container
		// for recalculation :-)
		//
		// This avoid needless work. The 'first' iterator is not
		// going to go anywhere...

		try {
			container->invoke_layoutmanager
				([&]
				 (const auto &l)
				 {
					 l->recalculate(IN_THREAD);
				 });
		} CATCH_EXCEPTIONS;

		// Ok, we can get rid of this.

		p->second.erase(first);

		if (p->second.empty())
			containers_2_recalculate_thread_only->erase(p);
	}
	return true;
}

bool connection_threadObj::process_element_position_updated(ONLY IN_THREAD,
							    int &poll_for)
{
	if (element_position_updated_thread_only->empty())
		return false;

	while (!element_position_updated_thread_only->empty())
	{
		auto e=next_highest_element(*element_position_updated_thread_only);

		LOG_TRACE("element_position_updated: " << e->objname()
			  << "(" << &*e << ")");
		try {
			auto &data=e->data(IN_THREAD);

			if (data.current_position !=
			    data.previous_position)
			{
				data.previous_position=data.current_position;
				e->process_updated_position(IN_THREAD);
			}
			else
			{
				e->process_same_position(IN_THREAD);
			}
		} CATCH_EXCEPTIONS;
	}
	return true;
}

bool connection_threadObj::redraw_elements(ONLY IN_THREAD, int &poll_for)
{
	if (elements_to_redraw_thread_only->empty())
		return false;

	while (!elements_to_redraw_thread_only->empty())
	{
		auto e=*elements_to_redraw_thread_only->begin();

		e->explicit_redraw(IN_THREAD);
	}

	return true;
}

LIBCXXW_NAMESPACE_END
