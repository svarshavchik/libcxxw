/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_thread_h
#define connection_thread_h

#include "connection_infofwd.H"
#include "connection_threadfwd.H"
#include "connection_info.H"
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>
#include <poll.h>

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


 public:

	// Stuff that's accessed by both the connection thread, and everytong else.

	class shared_data_info {
	public:

		// Currently active window handlers.

		std::map<xcb_window_t, ref<window_handlerObj>> window_handlers;

		// Destroyed xids, waiting until the corresponding
		// "destroyed" message is received from the display server.
		//
		// We can't get rid of them immediately, because it would
		// recycle these xids, and until a "destroyed" message is
		// received, a message related to the current xid could be
		// interpreted as related to the new xid. So, once something
		// is destroyed it goes here, until the destroyed message is
		// received.

		std::map<uint32_t, ref<xidObj>> destroyed_xids;
	};

	typedef mpobj<shared_data_info> shared_data_t;

	shared_data_t shared_data;


	// Event processing
 private:
	void recycle_xid(uint32_t xid);
};

#include "xid_t_connection_thread.H"

LIBCXXW_NAMESPACE_END

#endif
