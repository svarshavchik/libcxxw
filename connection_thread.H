#ifndef connection_thread_h
#define connection_thread_h

#include "connection.H"
#include <x/threadmsgdispatcher.H>
#include <x/logger.H>
#include <poll.h>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN connectionObj::implObj::threadObj
	: public threadmsgdispatcherObj {

	LOG_CLASS_SCOPE;

	const ref<infoObj> info;

 public:

	threadObj(const ref<infoObj> &info);
	~threadObj() noexcept;

	void run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin);

 private:

	/////////////////////////////////////////////////////////////////////
	//
	// Run something.

	void run_something(msgqueue_auto &msgqueue,
			   struct pollfd *topoll,
			   size_t &npoll);

	void run_event(const xcb_generic_event_t *event);
};

LIBCXXW_NAMESPACE_END

#endif
