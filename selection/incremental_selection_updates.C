/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "selection/incremental_selection_updates.H"
#include "assert_or_throw.H"
#include <x/property_value.H>

LIBCXXW_NAMESPACE_START
static property::value<hms>
incremental_selection_update_timeout(LIBCXX_NAMESPACE_STR
				     "::w::incremental_selection_update_timeout"
				     , hms(0, 5, 0));

tick_clock_t::time_point
connection_threadObj::incremental_selection_update_info::next_incremental_update_timeout()
{
	return tick_clock_t::now()+std::chrono::duration_cast
		<tick_clock_t::duration>
		(std::chrono::seconds
		 (incremental_selection_update_timeout.get().seconds()));
}

void connection_threadObj::incremental_selection_update_info
::no_more_pending_updates(ONLY IN_THREAD,
			  pending_updates_t::iterator iter)
{
	assert_or_throw(iter != pending_updates.end(),
			"Internal error: invalid pending_updates iterator.");

	// If this is not one of our windows,
	// we don't want to know about its property changes.

	auto w=iter->first;

	if (IN_THREAD->window_handlers(IN_THREAD)->find(w) ==
	    IN_THREAD->window_handlers(IN_THREAD)->end() &&
	    w != IN_THREAD->root_window(IN_THREAD))
		// We always monitor the root window for CWWTHEME changes.
	{
		uint32_t value=0;

		xcb_change_window_attributes(IN_THREAD->info->conn,
					     w, XCB_CW_EVENT_MASK,
					     &value);

	}
	timeouts.erase(iter->second.timeout_entry);
	pending_updates.erase(iter);
}

connection_threadObj::incremental_selection_update_info
::pending_window_updates_t &
connection_threadObj::incremental_selection_update_info
::get_updates_for_window(ONLY IN_THREAD, xcb_window_t w)
{
	return get_updates_for_window(IN_THREAD, w, pending_updates.find(w));
}

connection_threadObj::incremental_selection_update_info
::pending_window_updates_t &
connection_threadObj::incremental_selection_update_info
::get_updates_for_window(ONLY IN_THREAD,
			 xcb_window_t w,
			 pending_updates_t::iterator iter)
{
	// Each time we're asked to retrieve the pending_window_updates_t
	// we'll effectively reset the window's timeout.

	auto timeouts_iter=
		timeouts.insert({next_incremental_update_timeout(), w});

	try {
		if (iter==pending_updates.end())
		{
			iter=pending_updates.insert
				({w, pending_window_updates_t()}).first;

			// First time. If this is not one of our windows,
			// explicitly ask to be notified about its properties.

			if (IN_THREAD->window_handlers(IN_THREAD)->find(w) ==
			    IN_THREAD->window_handlers(IN_THREAD)->end())
			{
				uint32_t value=
					XCB_EVENT_MASK_PROPERTY_CHANGE;

				xcb_change_window_attributes(IN_THREAD->info
							     ->conn, w,
							     XCB_CW_EVENT_MASK,
							     &value);
			}
		}
		else
		{
			// Remove the existing timeout entry, before inserting
			// the new one, below.
			timeouts.erase(iter->second.timeout_entry);
		}
	} catch (...)
	{
		timeouts.erase(timeouts_iter);
		throw;
	}

	iter->second.timeout_entry=timeouts_iter;

	return iter->second;
}

// Connection thread:
//
// After we invoke_scheduled_callbacks(), we check if we need to
// expire_incremental_updates().

void connection_threadObj::expire_incremental_updates(ONLY IN_THREAD,
						      int &poll_for)
{
	auto &incremental_updates=
		*IN_THREAD->pending_incremental_updates(IN_THREAD);

	while (1)
	{
		// Is the first window in timeouts has expired?

		auto b=incremental_updates.timeouts.begin();
		auto e=incremental_updates.timeouts.end();

		if (b == e)
			break;

		auto now=tick_clock_t::now();

		if (b->first <= now)
		{
			// Yes, remove it, and look again.
			incremental_updates
				.no_more_pending_updates(IN_THREAD,
							 incremental_updates.
							 pending_updates.find
							 (b->second));
			continue;
		}

		// poll_for was set by invoke_scheduled_callbacks().
		//
		// compute_poll_until() the first window to expire, and update
		// poll_for, if needed.

		compute_poll_until(now, b->first, poll_for);
		break;
	}
}

void connection_threadObj
::handle_incremental_update(ONLY IN_THREAD,
			    const xcb_property_notify_event_t *event)
{
	// We might get property notification events not for our own windows
	// but for someone else's window when we're sending them incremental
	// updates, and they deleted the property, after receiving each
	// incremental update.
	//
	// Search incremental updates.

	if (event->state != XCB_PROPERTY_DELETE)
		return;

	auto &incremental_updates=
		*IN_THREAD->pending_incremental_updates(IN_THREAD);

	auto iter=incremental_updates.pending_updates.find(event->window);

	if (iter==incremental_updates.pending_updates.end())
		return;

	auto value_iter=iter->second.updates.find(event->atom);

	if (value_iter==iter->second.updates.end())
		return;

	auto v=value_iter->second->next_chunk(IN_THREAD);
	auto size=v->data_end-v->data_begin;

	LOG_DEBUG("Updated property "
		  << event->atom
		  << " of window " << event->window << " (incremental, "
		  << size << " bytes, format="
		  << (int)v->format << ")");

	xcb_change_property(info->conn,
			    XCB_PROP_MODE_APPEND,
			    event->window,
			    event->atom,
			    v->type,
			    v->format,
			    size/(v->format/8),
			    &*v->data_begin);

	if (size)
	{
		// Bump the timeout
		incremental_updates
			.get_updates_for_window(IN_THREAD, event->window, iter);
		return;
	}

	LOG_DEBUG("Finished updating property "
		  << event->atom
		  << " of window " << event->window << " (incremental)");

	// Last update

	iter->second.updates.erase(value_iter);
	if (!iter->second.updates.empty())
		return; // Something is still being updated.

	LOG_DEBUG("Disabling PropertyNotify for incremental selection updates");
	incremental_updates.no_more_pending_updates(IN_THREAD, iter);
}

LIBCXXW_NAMESPACE_END
