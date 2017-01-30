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
#include "x/w/rectangle.H"
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>
#include <x/functional.H>
#include <x/mpobj.H>
#include <x/weakptr.H>
#include <x/refptr_hash.H>
#include <functional>
#include <poll.h>
#include <unordered_map>
#include <unordered_set>
#include <set>

LIBCXXW_NAMESPACE_START

class window_handlerObj;
class xidObj;
class containers_2_recalculate_map;

class LIBCXX_HIDDEN connection_threadObj
	: public threadmsgdispatcherObj {

	LOG_CLASS_SCOPE;

 public:

	const connection_info info;

	connection_threadObj(const connection_info &info);
	~connection_threadObj();

	// Call get_error() and log the results.

	void report_error(const xcb_generic_error_t *e);

	void run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin);

 private:

	/////////////////////////////////////////////////////////////////////
	//
	// Run something.

	void run_something(msgqueue_auto &msgqueue,
			   struct pollfd *topoll,
			   size_t &npoll);

	void run_event(const xcb_generic_event_t *event);

	///////////////////////////////////////////////////////////////////
	//
	// These objects are constructed on the connection thread's stack,
	// and are accessible IN_THREAD_ONLY

	// Define a set of elements, ordered by their nesting level.

	typedef std::map<size_t, std::unordered_set<elementimpl>> element_set_t;

 public:
	static void insert_element_set(element_set_t &s,
				       const elementimpl &i);

	static elementimpl next_lowest_element(element_set_t &s);
	static elementimpl next_highest_element(element_set_t &s);
 private:
	// Here's a list of elements that have requested their visibility to
	// be changed.

	element_set_t *visibility_updated_thread_only;

	bool process_visibility_updated(IN_THREAD_ONLY);

	// Root window on screen 0

	xcb_window_t root_window_thread_only;

	// Callback for when CXXWTHEME on the root window change.s

	std::function<void ()> *cxxwtheme_changed_thread_only;

	// Active window handlers.

	std::unordered_map<xcb_window_t,
			   ref<window_handlerObj>> *window_handlers_thread_only;

	// Destroyed xids, waiting until the corresponding
	// "destroyed" message is received from the display server.
	//
	// We can't get rid of them immediately, because it would
	// recycle these xids, and until a "destroyed" message is
	// received, a message related to the current xid could be
	// interpreted as related to the new xid. So, once something
	// is destroyed it goes here, until the destroyed message is
	// received.

	std::unordered_map<uint32_t, ref<xidObj>> *destroyed_xids_thread_only;

	// Collected Expose event rectangles. They get collected until
	// a received expose event indicates count=0, then the exposure
	// event gets processed

	rectangle_set *exposed_rectangles_thread_only;

	// Elements that need to be redrawn

	typedef std::unordered_set<ref<elementObj::implObj>
				   > elements_to_redraw_set;

	elements_to_redraw_set *elements_to_redraw_thread_only;

	bool redraw_elements(IN_THREAD_ONLY);

	// Which containers need to be recalculated
	containers_2_recalculate_map *containers_2_recalculate_thread_only;

	bool recalculate_containers(IN_THREAD_ONLY);

	// Which elements' position has been updated. Rather than invoking
	// process_updated_position() immediately, they get cached here, and
	// processed.

	element_set_t *element_position_updated_thread_only;
	bool process_element_position_updated(IN_THREAD_ONLY);

	// on_disconnect() callback

	std::function<void ()> disconnect_callback_thread_only;

	// Event processing
	void recycle_xid(uint32_t xid);

 public:
	THREAD_DATA_ONLY(window_handlers);
	THREAD_DATA_ONLY(destroyed_xids);
	THREAD_DATA_ONLY(visibility_updated);
	THREAD_DATA_ONLY(elements_to_redraw);
	THREAD_DATA_ONLY(containers_2_recalculate);
	THREAD_DATA_ONLY(element_position_updated);

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
		do_run_as(file, line,
			  make_function<void (IN_THREAD_ONLY)>(std::forward<T>
							       (func)));
	}

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
};

#define connection_thread_included
#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
