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
	bool flag=false;

	// Start with the lower-most elements and work our way up to the
	// top level elements. Since until the top-level is visible anything
	// inside it is not visible, everything inherits visibility at the
	// last possible moment.

	lowest_sizable_elements_first
		(IN_THREAD,
		 *visibility_updated(IN_THREAD),
		 poll_for,
		 [&]
		 (const auto &e)
		 {
			 flag=true;

			 LOG_TRACE("update_visibility: " << e->objname()
				   << "(" << &*e << ")");
			 try {
				 e->update_visibility(IN_THREAD);
			 } CATCH_EXCEPTIONS;
		 });
	return flag;
}

bool connection_threadObj::recalculate_containers(ONLY IN_THREAD, int &poll_for)
{
	bool flag=false;

	for (auto b=containers_2_recalculate_thread_only->begin(),
		     e=containers_2_recalculate_thread_only->end(); b != e;)
	{
		--e;

		for (auto first=e->second.begin(), firste=e->second.end();
		     first != firste; ++first)
		{
			auto container=*first;

			if (resize_pending(IN_THREAD,
					   ref(&container
					       ->container_element_impl()),
					   poll_for))
				continue;

			// Wait until recalculate() is invoked before
			// removing it.

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

			flag=true;
			// Ok, we can get rid of this.

			e->second.erase(first);

			if (e->second.empty())
				containers_2_recalculate_thread_only->erase(e);

			b=containers_2_recalculate_thread_only->begin();
			e=containers_2_recalculate_thread_only->end();
			break;
		}
	}
	return flag;
}

bool connection_threadObj::process_element_position_updated(ONLY IN_THREAD,
							    int &poll_for)
{
	bool flag=false;

	// We start with the "highest", or the topmost element waiting for
	// its updated position to be processed, since when its resized it'll
	// usually update the position of all elements inside it, which have
	// a numerically higher nesting level, and we'll pick them up
	// immediately, right here, and so process everyone's updated position
	// in one pass.

	highest_sizable_elements_first
		(IN_THREAD,
		 *element_position_updated(IN_THREAD),
		 poll_for,
		 [&]
		 (const auto &e)
		 {
			 try {
				 auto &data=e->data(IN_THREAD);

				 if (data.current_position !=
				     data.previous_position)
				 {
					 data.previous_position=
						 data.current_position;
					 e->process_updated_position(IN_THREAD);
				 }
				 else
				 {
					 e->process_same_position(IN_THREAD);
				 }
			 } CATCH_EXCEPTIONS;
			 flag=true;
		 });

	return flag;
}

bool connection_threadObj::redraw_elements(ONLY IN_THREAD, int &poll_for)
{
	bool flag=false;

	for (auto b=elements_to_redraw(IN_THREAD)->begin(),
		     e=elements_to_redraw(IN_THREAD)->end(); b != e; )
	{
		auto p=*b;

		if (resize_pending(IN_THREAD, p, poll_for))
		{
			++b;
			continue;
		}

		// explicit_redraw() removes itself from elements_to_redraw,
		// just need to make sure we increment the iterator, here.
		++b;

		p->explicit_redraw(IN_THREAD);
		flag=true;
	}

	return flag;
}

LIBCXXW_NAMESPACE_END
