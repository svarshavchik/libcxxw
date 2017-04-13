/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_thread_h
#define connection_thread_h

#include "connection_infofwd.H"
#include "connection_threadfwd.H"
#include "x/w/batch_queuefwd.H"
#include "elementfwd.H"
#include "connection_info.H"
#include "draw_info_cachefwd.H"
#include "x/w/rectangle.H"
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <x/mcguffinmultimapfwd.H>
#include <x/mcguffincontainerobj.H>
#include <x/mpobj.H>
#include <x/weakptr.H>
#include <x/refptr_hash.H>
#include <functional>
#include <poll.h>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <atomic>
#include <chrono>

LIBCXXW_NAMESPACE_START

class window_handlerObj;
class xidObj;
class containers_2_recalculate_map;

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

	void run_something(msgqueue_auto &msgqueue,
			   struct pollfd *topoll,
			   size_t &npoll);

	//! Run an event received from the X server.

	void run_event(IN_THREAD_ONLY, const xcb_generic_event_t *event);

	//! Most recent timestamp from the server.

	xcb_timestamp_t timestamp_thread_only=XCB_CURRENT_TIME;
	///////////////////////////////////////////////////////////////////
	//
	// These objects are constructed on the connection thread's stack,
	// and are accessible IN_THREAD_ONLY

	//! Define a set of elements, ordered by their nesting level.

	typedef std::map<size_t, std::unordered_set<elementimpl>> element_set_t;

 public:
	THREAD_DATA_ONLY(timestamp);
	static void insert_element_set(element_set_t &s,
				       const elementimpl &i);

	static elementimpl next_lowest_element(element_set_t &s);
	static elementimpl next_highest_element(element_set_t &s);
 private:
	//! A list of elements that have requested their visibility to be changed.

	element_set_t *visibility_updated_thread_only;

	//! Check for a visibility change, and process it.

	bool process_visibility_updated(IN_THREAD_ONLY);

	//! Root window on screen 0

	xcb_window_t root_window_thread_only;

	// Callback for when CXXWTHEME on the root window changes.

	std::function<void ()> *cxxwtheme_changed_thread_only;

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

	//! Collecting exposure rectangles.

	//! They get collected until
	//! a received expose event indicates count=0, then the exposure
	//! event gets processed

	rectangle_set *exposed_rectangles_thread_only;

	//! A container for elements that need to be redrawn.

	typedef std::unordered_set<ref<elementObj::implObj>
				   > elements_to_redraw_set;


	//! Elements that need to be redrawn
	elements_to_redraw_set *elements_to_redraw_thread_only;

	//! Draw elements.
	bool redraw_elements(IN_THREAD_ONLY);

	//! Which containers need to be recalculated
	containers_2_recalculate_map *containers_2_recalculate_thread_only;

	//! Find a container to recalculate
	bool recalculate_containers(IN_THREAD_ONLY);

	//! Display elements that need their updated positions processed.

	//! Which elements' position has been updated. Rather than invoking
	//! process_updated_position() immediately, they get cached here, and
	//! processed.

	element_set_t *element_position_updated_thread_only;

	//! Process an element's position.
	bool process_element_position_updated(IN_THREAD_ONLY);

		//! A clock for regularly-scheduled callbacks.

	typedef std::chrono::steady_clock tick_clock_t;

	//! tick_clock_t's time_point.

	typedef tick_clock_t::time_point callback_time_point_t;

	//! A regularly-scheduled callback function object.

	typedef functionref<void (IN_THREAD_ONLY)> callback_functional_t;

	//! A container for currently scheduled callbacks.

	typedef mcguffinmultimap<callback_time_point_t,
				 callback_functional_t>
		scheduled_callbacks_t;

	//! Scheduled callbacks

	scheduled_callbacks_t *scheduled_callbacks_thread_only;

	class incremental_selection_update_info;

	//! Pending incremental selection updates we're sending.

	incremental_selection_update_info *pending_incremental_updates_thread_only;
	//! Check for any scheduled callbacks, and invoke them.

	bool invoke_scheduled_callbacks(IN_THREAD_ONLY, int &);

	//! Expire incremental selection updates

	void expire_incremental_updates(IN_THREAD_ONLY, int &);

	//! Handle an incremental update.

	void handle_incremental_update(IN_THREAD_ONLY,
				       const xcb_property_notify_event_t *);

	//! Compute timeout parameter for poll().

	//! Used by invoke_scheduled_callbacks() and
	//! expire_incremental_updates(), when now is before the next
	//! scheduled timeout, when. Computes the timeout parameter, in
	//! milliseconds, for poll().

	static int compute_poll_until(tick_clock_t::time_point now,
				      tick_clock_t::time_point when);
	//! Nothing to do.

	//! And when there's nothing else to do, just before waiting for
	//! events, check for any established grabs, and grab them.
	void allow_events(IN_THREAD_ONLY);

	//! on_disconnect() callback

	std::function<void ()> disconnect_callback_thread_only;

	//! Event processing
	void recycle_xid(uint32_t xid);

	//! Current draw_info cache object.

	draw_info_cache *current_draw_info_cache_thread_only;

 public:
	THREAD_DATA_ONLY(root_window);
	THREAD_DATA_ONLY(window_handlers);
	THREAD_DATA_ONLY(destroyed_xids);
	THREAD_DATA_ONLY(visibility_updated);
	THREAD_DATA_ONLY(elements_to_redraw);
	THREAD_DATA_ONLY(containers_2_recalculate);
	THREAD_DATA_ONLY(element_position_updated);
	THREAD_DATA_ONLY(current_draw_info_cache);
	THREAD_DATA_ONLY(scheduled_callbacks);
	THREAD_DATA_ONLY(pending_incremental_updates);

#include "connection_thread.msgs.H"

 public:

	// Wrapper for the do_run_as() message.

	// Invoke run_as() as follows, in order to execute a closure from
	// the connection thread:
	//
	//    run_as(RUN_AS, [](IN_THREAD_ONLY) { ... });

	template<typename T>
		void run_as(const char *file, int line, T &&func)
	{
		// Make sure all changes in the main execution thread are
		// committed by now. Although this should theoretically
		// taken care of by the mutex, this is technically required
		// for the connection thread to see what it needs to see.
		std::atomic_thread_fence(std::memory_order_release);

		do_run_as(file, line,
			  make_function<void (IN_THREAD_ONLY)>(std::forward<T>
							       (func)));
	}

	//! Have the connection thread execute a lambda at some point later.

	//! Takes a duration, and a lambda, and makes arrangements for the
	//! lambda to be invoked at the prescribed point in time.

	template<typename lambda, typename timeout_duration>
		ref<obj> schedule_callback(IN_THREAD_ONLY,
					   timeout_duration &&duration,
					   lambda &&l)
	{
		return do_schedule_callback(IN_THREAD,
					    std::chrono::duration_cast
					    <tick_clock_t::duration>
					    (std::forward<timeout_duration>
					     (duration)),
					    callback_functional_t::create
					    (std::forward<lambda>(l)));
	}

	//! Implement schedule_callback().

	ref<obj> do_schedule_callback(IN_THREAD_ONLY,
				      const tick_clock_t::duration &timeout,
				      const callback_functional_t &callback);

	//! Macro for logging purpose

#define RUN_AS	__FILE__, __LINE__

	// Politely stop this thread, after processing all messages,
	// and flushing all output.

	void stop() override;

 private:
	bool stop_received;
	bool stopping_politely;

	// Secondary queue for batched messages.

	active_queue_t batched_queue;

	mpobj<weakptr<batch_queueptr>> current_batch_queue;

 public:

	//! Same as run_as, but the job goes into the batch queue, instead.

	template<typename T>
		void run_as_batched(const char *file, int line, T &&func)
	{
		sendeventaux(batched_queue,
			     &connection_threadObj::dispatch_do_run_as,
			     this,
			     file, line,
			     make_function<void (IN_THREAD_ONLY)>
			     (std::forward<T>(func)));
	}

	void execute_batched_jobs()
	{
		process_events(batched_queue);
	}

	//! Return a batch_queue object, for batching jobs for the thread.

	batch_queue get_batch_queue();

	//! Destroy a window handler.

	//! This is scheduled for execution from generic_windowObj::implObj's
	//! destructor.
	//!
	//! The given handler is removed from window_handlers, and its xid_obj
	//! is moved into destroyed_xids, pending receipt of a destroy notify.

	void destroy_window_handler(IN_THREAD_ONLY,
				    const ref<window_handlerObj> &handler);

 private:
	//! Whether a MOTION_NOTIFY_EVENT has been buffered;
	bool motion_event_is_buffered=false;

	//! The buffered motion event
	xcb_motion_notify_event_t buffered_motion_event;

	//! We try to buffer up MOTION_EVENTs until they stop coming.

	bool process_buffered_motion_event(IN_THREAD_ONLY);
};

#define connection_thread_included
#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
