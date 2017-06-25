/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef batch_queueobj_h
#define batch_queueobj_h

#include "x/w/batch_queuefwd.H"
#include "x/w/layoutmanagerobj.H"
#include "connection_threadfwd.H"
#include <x/mpobj.H>
#include <x/ptrfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! An element that has a parent element.

class LIBCXX_HIDDEN batch_queueObj : virtual public obj {

	//! Something was scheduled...

	mpobj<bool> something_scheduled;

 public:

	//! My connection thread.

	const connection_thread my_thread;

	//! Constructor

	batch_queueObj(const connection_thread &my_thread);

	//! The destructor schedules all the jobs.

	~batch_queueObj();

	//! Schedule a container for recalculation

	//! All layout managers gets collected, and a single run_as()
	//! call gets batched, to invoke needs_recalculation() on all of them
	//! in one shot.

	void schedule_for_recalculation(const ref<layoutmanagerObj::implObj> &);

	//! Execute the functor in the connection thread.

	//! The execution gets delayed until this batch_queue object goes out
	//! of scope and gets destroyed.
	//!
	//! Invokes my_thread->run_as_batched, which queues up the message
	//! on the batch_queue. The destructor sends the execute_batched_jobs()
	//! message, that processes the batch_queue.

	template<typename functor>
		void run_as(const char *file, int line,
			    functor &&f);
};

LIBCXXW_NAMESPACE_END

#endif
