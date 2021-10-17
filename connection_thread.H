/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_thread_h
#define connection_thread_h

#include "window_handlerfwd.H"
#include "element_position_updatedfwd.H"
#include "xid_t_fwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/impl/clock.H"
#include "x/w/impl/updated_position_infofwd.H"
#include "x/w/batch_queuefwd.H"
#include "x/w/elementobj.H"
#include "x/w/generic_windowobj.H"
#include "connection_info.H"
#include "x/w/rectangle.H"
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/functionalrefptr.H>
#include <x/mcguffinmultimapfwd.H>
#include <x/mcguffincontainerobj.H>
#include <x/mpobj.H>
#include <x/weakptr.H>
#include <x/refptr_hash.H>
#include <x/functionalrefptr.H>
#include <poll.h>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <deque>
#include <atomic>
#include <optional>

LIBCXXW_NAMESPACE_START

//! The connection thread. Handles X server events. Does everything.
class LIBCXX_HIDDEN connection_threadObj
	: public threadmsgdispatcherObj {

	LOG_CLASS_SCOPE;

 public:

	//! Our connection.
	const connection_info info;

	//! Constructor
	connection_threadObj(const connection_info &info);

	//! Destructor
	~connection_threadObj();

	//! Call get_error() and log the results.

	void report_error(const xcb_generic_error_t *e);

	//! Execution thread!
	void run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin);

 private:

	/////////////////////////////////////////////////////////////////////
	//

	//! Run something.

	bool run_something(msgqueue_auto &msgqueue,
			   struct pollfd *topoll,
			   size_t &npoll,
			   int &poll_for);

	//! Run an event received from the X server.

	void run_event(ONLY IN_THREAD, const xcb_generic_event_t *event);

	//! Most recent timestamp from the server.

	xcb_timestamp_t timestamp_thread_only=XCB_CURRENT_TIME;
	///////////////////////////////////////////////////////////////////
	//
	// These objects are constructed on the connection thread's stack,
	// and are accessible ONLY IN_THREAD

	//! Define a set of elements, ordered by their nesting level.

	typedef std::map<size_t, std::unordered_set<element_impl>> element_set_t;

 public:
	THREAD_DATA_ONLY(timestamp);
	static void insert_element_set(element_set_t &s,
				       const element_impl &i);

	static bool is_element_in_set(element_set_t &s,
				      const element_impl &i);

 private:

	//! Cache for recording which windows' resize is pending.
	typedef std::unordered_map<generic_windowObj::handlerObj *, bool
				   > resize_pending_cache_t;

	//! A list of elements that have requested their visibility to be changed.

	element_set_t *visibility_updated_thread_only;

	//! Check for a visibility change, and process it.

	bool process_visibility_updated(ONLY IN_THREAD,
					resize_pending_cache_t &cache,
					int &poll_for);

	//! Root window on screen 0

	xcb_window_t root_window_thread_only;

	// Callback for when CXXWTHEME on the root window changes.

	functionref<void (ONLY IN_THREAD)> *cxxwtheme_changed_thread_only;

 public:

	//! Notify all top level windows about a new theme.

	void theme_updated(ONLY IN_THREAD);

 private:
	//! Active window handlers.

	std::unordered_map<xcb_window_t,
			   ref<window_handlerObj>> *window_handlers_thread_only;

	//! Zombies

	//! Destroyed xids, waiting until the corresponding
	//! "destroyed" message is received from the display server.
	//!
	//! We can't get rid of them immediately, because it would
	//! recycle these xids, and until a "destroyed" message is
	//! received, a message related to the current xid could be
	//! interpreted as related to the new xid. So, once something
	//! is destroyed it goes here, until the destroyed message is
	//! received.

	std::unordered_map<uint32_t, ref<xidObj>> *destroyed_xids_thread_only;

	//! A container for elements that need to be redrawn.

	typedef std::unordered_set<ref<elementObj::implObj>
				   > elements_to_redraw_set;

	//! Invoke timeout_selection_request and process_focus_updates.

	bool process_selection_and_focus_updates(ONLY IN_THREAD,
						 int &poll_for);

	//! Elements that need to be redrawn
	elements_to_redraw_set *elements_to_redraw_thread_only;

	//! Draw elements.
	bool redraw_elements(ONLY IN_THREAD,
			     resize_pending_cache_t &cache,
			     int &poll_for);

	//! Which containers need to be recalculated in bulk
	containers_2_recalculate_map *containers_2_recalculate_thread_only;

	//! Find a container to recalculate
	bool recalculate_containers(ONLY IN_THREAD,
				    resize_pending_cache_t &c,
				    int &poll_for);

	//! Widgets that need their updated positions processed.

	//! Which elements' position has been updated. Rather than invoking
	//! process_updated_position() immediately, they get cached here, and
	//! processed.

	element_position_updated_t *element_position_updated_thread_only;

	//! Notify widgets with updated positions.

	//! Invokes process_updated_position() or process_same_position(),
	//! as appropriate.
	bool process_element_position_updated(ONLY IN_THREAD, int &poll_for);

	//! Widgets that want to be notified when their position is finalized
	element_set_t *element_position_finalized_thread_only;

	//! Notify widgets that their positions have been finalized.

	//! Invokes process_finalized_position().
	bool process_element_position_finalized(ONLY IN_THREAD,
						int &poll_for);

	//! Determine which widgets can have thjeir contents moved directly.

	//! Internally used by process_element_position_updated() to
	//! find all widgets whose image can be moved directly in the window
	//! without redrawing them from scratch.

	rectarea move_updated_position_widgets
		(ONLY IN_THREAD,
		 updated_position_info &info,
		 const ref<generic_windowObj::handlerObj> &window,
		 all_updated_position_widgets_t &widgets,
		 std::unordered_set<element_impl> &moved,
		 std::unordered_set<element_impl> &to_redraw,
		 std::unordered_set<element_impl> &to_redraw_recursively);

 public:
	//! tick_clock_t's time_point.

	typedef tick_clock_t::time_point callback_time_point_t;

	//! Is this window being resized?

	//! Returns true if this window is pending to be resized by the
	//! window manager. There's a reasonable timeout that gets set, for
	//! that. If the timeout expires, something must be wrong so we
	//! forcefully clear the pending resize flag, and perform some
	//! cleanup.

	static bool check_resize_pending(ONLY IN_THREAD,
					 generic_windowObj::handlerObj &wh,
					 int &poll_for,
					 callback_time_point_t now);

 private:
	//! A regularly-scheduled callback function object.

	typedef functionref<void (ONLY IN_THREAD)> callback_functional_t;

	//! A container for currently scheduled callbacks.

	typedef mcguffinmultimap<callback_time_point_t,
				 callback_functional_t>
		scheduled_callbacks_t;

	//! Scheduled callbacks

	scheduled_callbacks_t *scheduled_callbacks_thread_only;

	class incremental_selection_update_info;

	//! Pending incremental selection updates we're sending.

	incremental_selection_update_info *pending_incremental_updates_thread_only;

	//! idle callbacks are executed last, just before flushing X events.

	typedef std::deque<functionref<void (ONLY IN_THREAD)>
			   > idle_callbacks_t;

	//! Pending idle callbacks.
	idle_callbacks_t *idle_callbacks_thread_only;

	//! Check for any scheduled callbacks, and invoke them.

	bool invoke_scheduled_callbacks(ONLY IN_THREAD, int &);

	//! Expire incremental selection updates

	void expire_incremental_updates(ONLY IN_THREAD, int &);

	//! Handle an incremental update.

	void handle_incremental_update(ONLY IN_THREAD,
				       const xcb_property_notify_event_t *);

	//! Look for something in idle_callbacks to run, instead of sleeping.

	bool run_idle(ONLY IN_THREAD);

 public:
	//! Compute timeout parameter for poll().

	//! Used by invoke_scheduled_callbacks() and
	//! expire_incremental_updates(), when now is before the next
	//! scheduled timeout, when. Computes the timeout parameter, in
	//! milliseconds, for poll().
	//!
	//! Also used to set the timeout for receiving a ConfigureNotify
	//! event from the window manager accomodating our update hints for
	//! our minimum and maximum size.

	static void compute_poll_until(tick_clock_t::time_point now,
				       tick_clock_t::time_point when,
				       int &poll_for);
 private:
	//! Nothing to do.

	//! Before redrawing anything check if we have received any
	//! ConfigureNotify events, and process them.

	bool process_pending_configure_events(ONLY IN_THREAD);

	//! Nothing to do.

	//! Before redrawing anything check if we have received any
	//! Exposure events, and process them.

	bool process_pending_exposure_events(ONLY IN_THREAD);

	//! Still nothing to do.

	//! We can release any grabs now.

	void release_grabs(ONLY IN_THREAD);

	//! on_disconnect() callback

	functionref<void ()> disconnect_callback_thread_only;

	//! Flag that gets set upon a disconnect.
	bool disconnected_flag_thread_only;

	//! Event processing
	void recycle_xid(uint32_t xid);

 public:
	THREAD_DATA_ONLY(root_window);
	THREAD_DATA_ONLY(window_handlers);
	THREAD_DATA_ONLY(destroyed_xids);
	THREAD_DATA_ONLY(visibility_updated);
	THREAD_DATA_ONLY(elements_to_redraw);
	THREAD_DATA_ONLY(containers_2_recalculate);
	THREAD_DATA_ONLY(element_position_updated);
	THREAD_DATA_ONLY(element_position_finalized);
	THREAD_DATA_ONLY(scheduled_callbacks);
	THREAD_DATA_ONLY(pending_incremental_updates);
	THREAD_DATA_ONLY(idle_callbacks);

#include "connection_thread.msgs.H"

 public:

	// Wrapper for the do_run_as() message.

	// Invoke run_as() as follows, in order to execute a closure from
	// the connection thread:
	//
	//    run_as([](ONLY IN_THREAD) { ... });

	template<typename T>
		void run_as(T &&func)
	{
		// Make sure all changes in the main execution thread are
		// committed by now. Although this should theoretically
		// taken care of by the mutex, this is technically required
		// for the connection thread to see what it needs to see.
		std::atomic_thread_fence(std::memory_order_release);

		do_run_as(make_function<void (ONLY IN_THREAD)>(std::forward<T>
							       (func)));
	}

	//! Have the connection thread execute a lambda at some point later.

	//! Takes a duration, and a lambda, and makes arrangements for the
	//! lambda to be invoked at the prescribed point in time.

	template<typename lambda, typename timeout_duration>
		ref<obj> schedule_callback(ONLY IN_THREAD,
					   timeout_duration &&duration,
					   lambda &&l)
	{
		return do_schedule_callback(IN_THREAD,
					    std::chrono::duration_cast
					    <tick_clock_t::duration>
					    (std::forward<timeout_duration>
					     (duration)),
					    std::forward<lambda>(l));
	}

	//! Implement schedule_callback().

	ref<obj> do_schedule_callback(ONLY IN_THREAD,
				      const tick_clock_t::duration &timeout,
				      const callback_functional_t &callback);

	// Politely stop this thread, after processing all messages,
	// and flushing all output.

	void stop() override;

 private:
	bool stop_received;

	// Secondary queue for batched messages.

	active_queue_t batched_queue;

	// batch_queue singleton.

	mpobj<weakptr<batch_queueptr>> current_batch_queue;

 public:
	//! Same as run_as, but the job goes into the batch queue, instead.

	template<typename T>
		void run_as_batched(T &&func)
	{
		sendeventaux(batched_queue,
			     &connection_threadObj::dispatch_do_run_as,
			     this,
			     make_function<void (ONLY IN_THREAD)>
			     (std::forward<T>(func)));
	}

	//! Return a batch_queue object, for batching jobs for the thread.

	batch_queue get_batch_queue();

	//! Install a window handler

	//! This gets scheduled for execution from generic_windowObj::implObj's
	//! constructor.

	//! The given handler gets added to window_handlers.

	void install_window_handler(ONLY IN_THREAD,
				    const ref<window_handlerObj> &handler);

	//! Uninstall a window handler.

	//! This gets scheduled for execution from generic_windowObj::implObj's
	//! destructor.
	//!
	//! The given handler is removed from window_handlers, and its xid_obj
	//! is moved into destroyed_xids, pending receipt of a destroy notify.

	void uninstall_window_handler(ONLY IN_THREAD,
				      const ref<window_handlerObj> &handler);

 private:
	//! Whether a MOTION_NOTIFY_EVENT has been buffered.
	std::optional<xcb_motion_notify_event_t> buffered_motion_event;

	//! We try to buffer up MOTION_EVENTs until they stop coming.

	bool process_buffered_motion_event(ONLY IN_THREAD);
};

#define connection_thread_included
#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
