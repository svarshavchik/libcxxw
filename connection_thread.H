/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_thread_h
#define connection_thread_h

#include "connection_infofwd.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"
#include "connection_info.H"
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>
#include <x/functional.H>
#include <functional>
#include <poll.h>
#include <map>
#include <set>

LIBCXXW_NAMESPACE_START

class window_handlerObj;
class xidObj;

class LIBCXX_HIDDEN connection_threadObj
	: public threadmsgdispatcherObj {

	LOG_CLASS_SCOPE;

 public:

	const connection_info info;

	connection_threadObj(const connection_info &info);
	~connection_threadObj() noexcept;

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

	// Define a set of elements, ordered by their nesting level.

	typedef std::set<elementimpl, elements_by_nesting_level> element_set_t;

	// Here's a list of elements that have requested their visibility to
	// be changed.

	element_set_t *visibility_updated_thread_only;

	///////////////////////////////////////////////////////////////////
	//
	// These objects are constructed on the connection thread's stack,
	// and are accessible IN_THREAD_ONLY

	// Active window handlers.

	std::map<xcb_window_t, ref<window_handlerObj>> *window_handlers_thread_only;

	// Destroyed xids, waiting until the corresponding
	// "destroyed" message is received from the display server.
	//
	// We can't get rid of them immediately, because it would
	// recycle these xids, and until a "destroyed" message is
	// received, a message related to the current xid could be
	// interpreted as related to the new xid. So, once something
	// is destroyed it goes here, until the destroyed message is
	// received.

	std::map<uint32_t, ref<xidObj>> *destroyed_xids_thread_only;

	// on_disconnect() callback

	std::function<void ()> disconnect_callback_thread_only;

	// Event processing
	void recycle_xid(uint32_t xid);

 public:
	THREAD_DATA_ONLY(window_handlers);
	THREAD_DATA_ONLY(destroyed_xids);
	THREAD_DATA_ONLY(visibility_updated);

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

};

#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
