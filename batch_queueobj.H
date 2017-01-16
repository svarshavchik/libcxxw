/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef batch_queueobj_h
#define batch_queueobj_h

#include "x/w/batch_queuefwd.H"
#include "connection_threadfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! An element that has a parent element.

class LIBCXX_HIDDEN batch_queueObj : virtual public obj {

 public:

	//! My connection thread.

	const connection_thread my_thread;

	//! Constructor

	batch_queueObj(const connection_thread &my_thread);

	//! The destructor schedules all the jobs.

	~batch_queueObj();

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
