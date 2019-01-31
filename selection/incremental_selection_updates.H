/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef incremental_selection_updates_H
#define incremental_selection_updates_H

#include "connection_thread.H"
#include "selection/current_selection.H"

#include <map>
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! Pending incremental updates.

//! Object owned by the execution thread, and instantiated on the execution
//! thread's stack.
//!
//! Keeps track of pending incremental updates in the process of being
//! sent.
//!
//! A window requests a selection update, and we determine to send the window
//! an incremental update.
//!
//! If pending_updates does not already have an entry for this window, we
//! add it, and request property notifications for the window.
//!
//! Each entry in pending_updates has a corresponding entry in timeouts.
//! If no acknowledgement is received, from the window, when its timeout
//! expires, we remove its pending_updates entry.
//!
//! Its pending_updates entry also gets removed after the last incremental
//! update, for the window is fully sent, at which time we will stop the
//! property notification requests.

class LIBCXX_HIDDEN connection_threadObj::incremental_selection_update_info {

 public:

	//! Timeouts.

	//! When we don't receive a response from a property update, after
	//! some time drop all updates for this window.

	typedef std::multimap<tick_clock_t::time_point, xcb_window_t> timeouts_t;

	//! All currently scheduled timeouts.

	timeouts_t timeouts;

	//! All pending incremental updates for a given window.

	class pending_window_updates_t {

	public:
		//! All updates.
		std::unordered_map<xcb_atom_t,
			 ref<current_selectionObj::convertedValueObj>
			 > updates;

		//! When this window's updates timeout.

		timeouts_t::iterator timeout_entry;
	};

	//! A container for all windows with pending incremental updates.

	typedef std::unordered_map<xcb_window_t,
			 pending_window_updates_t> pending_updates_t;

	//! All windows with pending incremental updates

	pending_updates_t pending_updates;

	//! Compute the next timeout.

	static tick_clock_t::time_point next_incremental_update_timeout();

	pending_window_updates_t &get_updates_for_window(ONLY IN_THREAD,
							 xcb_window_t);
	pending_window_updates_t
		&get_updates_for_window(ONLY IN_THREAD,
					xcb_window_t,
					pending_updates_t::iterator);

	void no_more_pending_updates(ONLY IN_THREAD,
				     pending_updates_t::iterator iter);
};

LIBCXXW_NAMESPACE_END

#endif
