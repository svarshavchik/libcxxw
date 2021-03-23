/*
** Copyright 2017-2021 Double Precision, Inc.
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
#include "pixmap.H"
#include "catch_exceptions.H"
#include <x/refptr_hash.H>
#include <x/visitor.H>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <charconv>

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
	return std::visit(visitor{
			[](const not_resizing &)
			{
				return false;
			}, [&](const tick_clock_t::time_point &resizing_timeout)
			{
				auto now=tick_clock_t::now();

				if (now >= resizing_timeout)
				{
					wh.resizing(IN_THREAD)=not_resizing{};
					wh.invoke_stabilized(IN_THREAD);
					return false;
				}

				connection_threadObj::compute_poll_until
					(now,
					 resizing_timeout,
					 poll_for);

				return true;
			}},
		wh.resizing(IN_THREAD));
}

namespace {

	// Cache the calls to resize_pending(). They're expensive.
	// So we use an unordered_map to cache the results of resize_pending
	// by windowObj::handlerObj, and use try_emplace with this object
	// that has an operator bool that calls it. So, once the cache
	// already has this operator bool, no more calls get made.

	struct get_resize_pending {

		ONLY IN_THREAD;
		generic_windowObj::handlerObj &wh;
		int &poll_for;

		operator bool() const
		{
			return resize_pending(IN_THREAD, wh, poll_for);
		}
	};
}

void connection_threadObj
::do_lowest_sizable_element_first(ONLY IN_THREAD,
				  element_set_t &s,
				  resize_pending_cache_t &is_resize_pending,
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

			auto &wh=element->get_window_handler();

			if (is_resize_pending
			    .try_emplace(&wh,
					 get_resize_pending{
						 IN_THREAD,
							 wh,
							 poll_for
					 }).first->second)
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
						      resize_pending_cache_t
						      &is_resize_pending,
						      int &poll_for)
{
	CONNECTION_TRAFFIC_LOG("process visibility", *this);

	bool flag=false;

	// Start with the lower-most elements and work our way up to the
	// top level elements. Since until the top-level is visible anything
	// inside it is not visible, everything inherits visibility at the
	// last possible moment.

	lowest_sizable_elements_first
		(IN_THREAD,
		 *visibility_updated(IN_THREAD),
		 is_resize_pending,
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

bool connection_threadObj::recalculate_containers(ONLY IN_THREAD,
						  resize_pending_cache_t
						  &is_resize_pending,
						  int &poll_for)
{
	CONNECTION_TRAFFIC_LOG("recalculate", *this);

	bool flag=false;

	for (auto b=containers_2_recalculate_thread_only->begin(),
		     e=containers_2_recalculate_thread_only->end(); b != e;)
	{
		--e;

		for (auto first=e->second.begin(), firste=e->second.end();
		     first != firste; ++first)
		{
			auto container=*first;

			auto &wh=container->container_element_impl()
				.get_window_handler();

			if (is_resize_pending
			    .try_emplace(&wh,
					 get_resize_pending{
						 IN_THREAD,
							 wh,
							 poll_for
					 }).first->second)
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

// Factored out for readability.

inline rectarea connection_threadObj::move_updated_position_widgets
(ONLY IN_THREAD,
 updated_position_info &info,
 const ref<generic_windowObj::handlerObj> &wh,
 all_updated_position_widgets_t &widgets,
 std::unordered_set<element_impl> &moved,
 std::unordered_set<element_impl> &to_redraw,
 std::unordered_set<element_impl> &to_redraw_recursively)
{
	// All the moved area.

	rectarea to_flush;

	// Find widgets whose contents can_be_moved(), as is.

	updated_position_container_t move_container;

	for (auto level_b=widgets.begin(),
		     level_e=widgets.end(); level_b != level_e; ++level_b)
		for (auto iter_b=level_b->second.begin(),
			     iter_e=level_b->second.end();
		     iter_b != iter_e; ++iter_b)
		{
			auto &[e,position,moved_flag]=*iter_b;

			e->can_be_moved(IN_THREAD, {level_b, iter_b},
					move_container);
		}

	// Now figure out which way we're moving the movable widgets

	updated_position_move_info::summary move_summary;

	for (const auto &w:move_container)
		std::get<1>(w).where([&]
				     (auto direction)
				     {
					     ++(move_summary.*direction);
				     });
	move_summary.set_chosen();

	// Now, remove everything from move_container except widgets moving
	// in the chosen direction.

	auto [chosen_direction,
	      chosen_direction_comparator]=move_summary.chosen;

	move_container.erase
		(std::remove_if
		 (move_container.begin(),
		  move_container.end(),
		  [&]
		  (const auto &w)
		  {
			  bool do_remove=true;

			  std::get<1>(w).where
				  ([&]
				   (auto direction)
				   {
					   if (direction == chosen_direction)
						   do_remove=false;
				   });

			  return do_remove;
		  }), move_container.end());

	// Sort the widgets to move in the right order.
	//
	// If we're moving the widgets in the north-westerly direction we
	// want to move them starting with the most north-western widget,
	// and so on. It's possible that some other widget that's also being
	// moved north-west ends up in the same position, so we want to
	// move the most north-western one first, and then move the other one
	// into the space that got vacated.

	std::sort(move_container.begin(),
		  move_container.end(),
		  [=]
		  (const auto &a, const auto &b)
		  {
			  const auto &[a_iterator, a_info]=a;
			  const auto &[b_iterator, b_info]=b;

			  return chosen_direction_comparator(a_info, b_info);
		  });

	// Now, all the widgets in the move_container can be moved and have
	// their existing contents directly copied in the window_pixmap,
	// instead of redrawing them from scratch.

	for (const auto &w:move_container)
	{
		const auto &[iterator, move_info]=w;

		const auto &[all_levels_iterator, level_iterator]=iterator;

		auto &[e, new_position, widget_moved]=*level_iterator;

		auto &data=e->data(IN_THREAD);

		try {
			// This widget was process_updated_position()'ed,
			// so we must call this:
			e->redraw_after_process_updated_position(IN_THREAD,
								 info);
			// We will remove it from the window_buckets below.

		} CATCH_EXCEPTIONS;

		// Make sure that previous_position gets
		// what current_position is, *right now*.

		data.previous_position=new_position;

		// Copy the widget in the window_pixmap, then
		// insert the new widget position into to_flush.
		//
		// We can't simply copy the contents of the window drawable
		// directly. It's possible that the window became smaller
		// and we are now moving the widget into the smaller space,
		// and we still have the pixels cached in the window_pixmap,
		// but they're no longer in the drawable!

		wh->drawing_to_window_picture(IN_THREAD,
					      {move_info.move_to_x,
					       move_info.move_to_y,
					       move_info.scroll_from.width,
					       move_info.scroll_from.height
					      });
		auto &pixmap_impl=
			wh->window_pixmap(IN_THREAD)->impl;
		wh->copy_configured(move_info.scroll_from,
				    move_info.move_to_x,
				    move_info.move_to_y,
				    pixmap_impl,
				    pixmap_impl);
		to_flush.push_back
			({move_info.move_to_x,
			  move_info.move_to_y,
			  move_info.scroll_from.width,
			  move_info.scroll_from.height});
#ifdef DEBUG_MOVE_LOG
		DEBUG_MOVE_LOG();
#endif
		// Record this widget as being moved, and remove the widgetr
		// from widgets.elements, since we processed it here. All
		// other widgets.elements can processed below.
		moved.insert(e);
		widget_moved=true;
		CONNECTION_THREAD_ACTION_FOR("process position", &*e);
	}

	return to_flush;
}

namespace {
#if 0
}
#endif

//! Helper class used to find all repositionable widgets.

//! All widgets in the same window get bucketed together.

struct reposition_bucket {

	//! If the window's resize_pending(), skip all of its widgets

	//! The widgets list will be empty.
	const bool resize_pending_flag;

	//! All widgets that can be repositioned here.
	all_updated_position_widgets_t widgets;

	reposition_bucket(ONLY IN_THREAD,
			  const ref<generic_windowObj::handlerObj> &wh,
			  int &poll_for)
		: resize_pending_flag{resize_pending(IN_THREAD, *wh, poll_for)}
	{
	}
};

#if 0
{
#endif
}

bool connection_threadObj::process_element_position_updated(ONLY IN_THREAD,
							    int &poll_for)
{
	CONNECTION_TRAFFIC_LOG("process position", *this);

	bool flag=false;

	std::unordered_set<element_impl> moved;
	std::unordered_set<element_impl> to_redraw;
	std::unordered_set<element_impl> to_redraw_recursively;
	std::unordered_map<ref<generic_windowObj::handlerObj>,
			   rectarea> to_flush;

	// We start with the "highest", or the topmost element waiting for
	// its updated position to be processed, since when its resized it'll
	// usually update the position of all elements inside it, which have
	// a numerically higher nesting level, and we'll pick them up
	// immediately, right here, and so process everyone's updated position
	// in one pass.

	std::unordered_map<ref<generic_windowObj::handlerObj>,
			   reposition_bucket> window_buckets;

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

			auto e=*parent_b;

			// If this widget's window is resize_pending(), skip
			// it entirely.
			auto wh=ref{&e->get_window_handler()};

			auto &bucket=window_buckets.try_emplace
				(wh, IN_THREAD, wh, poll_for).first->second;

			if (bucket.resize_pending_flag)
				continue;

			// One way or another this widget's getting processed.
			//
			// So remove it from this nesting level's width.
			level_b->second.erase(parent_b);

			auto &data=e->data(IN_THREAD);

			try {
				// NOTE: scroll_by_parent_container()
				// short-circuits this processing.

				if (data.current_position
				    != data.previous_position)
				{
					bucket.widgets[e->nesting_level]
						.emplace_back
						(e, data.current_position,
						 false);

					e->process_updated_position(IN_THREAD);
				}
				else
				{
					e->process_same_position(IN_THREAD);
				}
			} CATCH_EXCEPTIONS;
			flag=true;
		}

		// If we processed all widgets at this nesting level, remove
		// it.
		if (level_b->second.empty())
			set.erase(level_b);
	}

	// We now go through all window_buckets, and see which widgets'
	// in each window can be moved without redrawing its contents.
	//
	// move_updated_position_widgets() will remove each moved widget
	// from its window_bucket, and we handle the rest below.

	updated_position_info info;

	for (auto &bucket:window_buckets)
	{
		if (bucket.second.resize_pending_flag)
			continue;

		auto r=move_updated_position_widgets(IN_THREAD,
						     info,
						     bucket.first,
						     bucket.second.widgets,
						     moved,
						     to_redraw,
						     to_redraw_recursively);

		if (!r.empty())
			to_flush.emplace(bucket.first, std::move(r));
	}

	// For all remaining widgets, we will redraw them.
	info.moved_how=info.moved_without_contents;

	for (auto &bucket:window_buckets)
	{
		if (bucket.second.resize_pending_flag)
			continue;
		for (auto &level:bucket.second.widgets)
			for (auto &widget:level.second)
			{
				auto &[e, new_position, moved] = widget;

				if (moved)
					// move_updated_position-widgets
					// handled this one.
					continue;
				auto &data=e->data(IN_THREAD);

				e->redraw_after_process_updated_position
					(IN_THREAD, info);

				// If the widget's position has changed
				// we need to have this widget and all
				// of its inner widgets redrawn, this
				// widget needs to_redraw_recursively.
				//
				// if the widget's position has not changed,
				// only its size changed, we only need
				// to_redraw the widget itself. Any child
				// widget's redrawing needs will be determined
				// separately, on their own merits.
				if (data.previous_position.x != new_position.x
				    ||
				    data.previous_position.y != new_position.y)
					to_redraw_recursively.insert(e);
				else
					to_redraw.insert(e);

				data.previous_position=new_position;
			}
	}
	// Immediately flush the areas that were moved.
	// This gives better results when a window gets resized because
	// widgets were inserted in the middle, somewhere. The moved areas
	// get redrawn quickly, and more expensive redraws get done
	// afterwards.

	for (auto &flush_windows:to_flush)
	{
		flush_windows.first->flush_redrawn_areas(IN_THREAD,
							 flush_windows.second);
	}

	// If the moved element needs to get moved again, make sure it
	// knows it has a scrollable rectangle, once more.
	for (auto &moved_element:moved)
	{
		moved_element->drawn(IN_THREAD);
	}

	// Now that we moved all widgets, schedule their redrawal, as
	// needed.
	for (const auto &e:to_redraw)
	{
		e->schedule_full_redraw(IN_THREAD);
	}

	for (const auto &e:to_redraw_recursively)
	{
		e->schedule_redraw_recursively(IN_THREAD, moved);
	}

	return flag;
}


bool connection_threadObj::redraw_elements(ONLY IN_THREAD,
					   resize_pending_cache_t
					   &is_resize_pending,
					   int &poll_for)
{
	CONNECTION_TRAFFIC_LOG("redraw", *this);

	std::vector<std::tuple<element_impl, redraw_priority_t>
		    > redraw_queue;

	redraw_queue.reserve(elements_to_redraw(IN_THREAD)->size());

	for (auto b=elements_to_redraw(IN_THREAD)->begin(),
		     e=elements_to_redraw(IN_THREAD)->end(); b != e; )
	{
		auto p=*b;

		++b;

		auto &wh=p->get_window_handler();

		if (is_resize_pending.try_emplace(&wh,
						  get_resize_pending{
							  IN_THREAD,
							  wh,
							  poll_for
						  }).first->second)
			continue;

		redraw_queue.push_back
			(std::tuple{p, p->get_redraw_priority(IN_THREAD) -
					p->get_window_handler().nesting_level
					* 16});
	}

	std::sort(redraw_queue.begin(),
		  redraw_queue.end(),
		  []
		  (const auto &a,
		   const auto &b)
		  {
			  return std::get<redraw_priority_t>(a) <
				  std::get<redraw_priority_t>(b);
		  });

	bool flag=false;

	for (auto b=redraw_queue.begin(), e=redraw_queue.end(),
		     cur_priority=b; b != e; ++b)
	{
		if (std::get<redraw_priority_t>(*cur_priority) !=
		    std::get<redraw_priority_t>(*b))
		{
#if 0
			// This is disabled. If flushing occurs at priority
			// order, flush_redrawn_areas will have fewer
			// opportunities to coalesce adjacent rectangles into
			// a single flush operation.
			//
			// This can be determined by looking at how many
			// get copied by flush_redrawn_areas when using
			// a creator to look at a color gradient and adding
			// a row.

			for (const auto &wh:*window_handlers(IN_THREAD))
				wh.second->flush_redrawn_areas(IN_THREAD);
#endif

			cur_priority=b;
		}

		auto &p=std::get<element_impl>(*b);

		try {
			CONNECTION_TRAFFIC_LOG("   redraw("
					       + p->objname() + ")", *this);
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
