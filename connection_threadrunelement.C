/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_thread.H"
#include "connection_thread_debug.H"
#include "element_position_updated.H"
#include "generic_window_handler.H"
#include "x/w/impl/element.H"
#include "x/w/impl/container.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/updated_position_info.H"
#include "catch_exceptions.H"
#include <x/refptr_hash.H>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

void connection_threadObj::insert_element_set(element_set_t &s,
					      const element_impl &i)
{
	s[i->nesting_level].insert(i);
}

bool connection_threadObj::is_element_in_set(element_set_t &s,
					     const element_impl &i)
{
	auto iter=s.find(i->nesting_level);

	if (iter != s.end())
		return false;

	return (iter->second.find(i) != iter->second.end());
}

// If we expect the widget's window to get resized we'll skip some
// processing until it does.

static bool resize_pending(ONLY IN_THREAD,
			   generic_windowObj::handlerObj &wh, int &poll_for)
{
	if (!wh.resizing(IN_THREAD))
		return false;

	auto now=tick_clock_t::now();

	if (now >= wh.resizing_timeout(IN_THREAD))
	{
		wh.resizing(IN_THREAD)=false;
		wh.invoke_stabilized(IN_THREAD);
		return false;
	}

	connection_threadObj::compute_poll_until(now,
						 wh.resizing_timeout(IN_THREAD),
						 poll_for);

	return true;
}

static bool resize_pending(ONLY IN_THREAD, const element_impl &e, int &poll_for)
{
	return resize_pending(IN_THREAD, e->get_window_handler(), poll_for);
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
			 CONNECTION_THREAD_ACTION_FOR("process visibility",
						      &*e);

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
					   container->container_element_impl()
					   .get_window_handler(),
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

			CONNECTION_THREAD_ACTION_FOR("recalculate",
						     &*container);
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

	auto &set=element_position_updated(IN_THREAD)->set(IN_THREAD);

	for (auto level_b=set.begin(), level_e=set.end(), p=level_b;
	     level_b != level_e; level_b=p)
	{
		++p;

		// Process each container's repositioned widgets.
		for (auto parent_b=level_b->second.begin(),
			     parent_e=level_b->second.end(), p=parent_b;
		     parent_b != parent_e; parent_b=p)
		{
			++p;

			updated_position_info info;

			// Process each widget, one at a time.

			for (auto e_b=parent_b->second.begin(),
				     e_e=parent_b->second.end(),
				     p=e_b;
			     e_b != e_e; e_b=p)
			{
				++p;

				auto &e=*e_b;

				if (resize_pending(IN_THREAD,
						   e->get_window_handler(),
						   poll_for))
					// Everyone is in the same container.
					break;

				try {
					auto &data=e->data(IN_THREAD);

					// NOTE: scroll_by_parent_container()
					// short-circuits this processing.

					if (data.current_position !=
					    data.previous_position)
					{
						// Make sure that
						// previous_position gets
						// what current_position is,
						// *right now*.

						auto new_position=
							data.current_position;

						e->process_updated_position
							(IN_THREAD, info);
						e->schedule_redraw_recursively
							(IN_THREAD);

						data.previous_position=
							new_position;
					}
					else
					{
						e->process_same_position
							(IN_THREAD);
					}
				} CATCH_EXCEPTIONS;
				CONNECTION_THREAD_ACTION_FOR("process position",
							     &*e);
				flag=true;

				parent_b->second.erase(e_b);
			}

			// If we processed all widgets in this container,
			// delete it off the list.
			if (parent_b->second.empty())
				level_b->second.erase(parent_b);
		}

		// If we processed all widgets at this nesting level, remove
		// it.
		if (level_b->second.empty())
			set.erase(level_b);
	}

	return flag;
}

bool connection_threadObj::redraw_elements(ONLY IN_THREAD, int &poll_for)
{
	bool flag=false;

	for (auto b=elements_to_redraw(IN_THREAD)->begin(),
		     e=elements_to_redraw(IN_THREAD)->end(); b != e; )
	{
		auto p=*b;

		++b;

		if (resize_pending(IN_THREAD, p, poll_for))
			continue;

		try {
			p->explicit_redraw(IN_THREAD);
		} CATCH_EXCEPTIONS;

		CONNECTION_THREAD_ACTION_FOR("redraw", &*p);
		flag=true;
	}

	// Now that's everything's been drawn to each window's pixmap buffer,
	// flush all the redrawn areas.
	for (const auto &wh:*window_handlers(IN_THREAD))
		wh.second->flush_redrawn_areas(IN_THREAD);

	return flag;
}

LIBCXXW_NAMESPACE_END
