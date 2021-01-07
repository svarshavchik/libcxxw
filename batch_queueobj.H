/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef batch_queueobj_h
#define batch_queueobj_h

#include "x/w/batch_queuefwd.H"
#include "x/w/layoutmanagerobj.H"
#include "x/w/elementobj.H"
#include "x/w/connection_threadfwd.H"
#include <x/mpobj.H>
#include <x/ptrfwd.H>
#include <x/logger.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! An element that has a parent element.

class LIBCXX_HIDDEN batch_queueObj : virtual public obj {

	//! My logger
	LOG_CLASS_SCOPE;

	//! Batched
 public:

	//! My connection thread.

	const connection_thread my_thread;

	//! Constructor

	batch_queueObj(const connection_thread &my_thread);

	//! The destructor schedules all the jobs.

	~batch_queueObj();

	//! Schedule an element to have its visibility update.

	void schedule_for_visibility(const ref<elementObj::implObj> &e,
				     bool visibility);

	//! Execute the functor in the connection thread.

	//! The execution gets delayed until this batch_queue object goes out
	//! of scope and gets destroyed.
	//!
	//! Invokes my_thread->run_as_batched, which queues up the message
	//! on the batch_queue. The destructor sends the execute_batched_jobs()
	//! message, that processes the batch_queue.

	template<typename functor>
		void run_as(functor &&f);
};

LIBCXXW_NAMESPACE_END

#endif
