/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "batch_queue.H"
#include "connection_thread.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/container.H"
#include "x/w/impl/element.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::batch_queueObj);

LIBCXXW_NAMESPACE_START

batch_queueObj::batch_queueObj(const connection_thread &my_thread)
	: my_thread(my_thread)
{
	LOG_DEBUG("Created " << this);
}

void batch_queueObj
::schedule_for_visibility(const ref<elementObj::implObj> &e,
			  bool visibility)
{
	my_thread->run_as([=, me=ref{this}]
			  (ONLY IN_THREAD)
			  {
				  e->request_visibility(IN_THREAD,
							visibility);
			  });
}

batch_queueObj::~batch_queueObj()
{
	LOG_DEBUG("Destroyed " << this);

	// This has the effect of executing the batch jobs and all
	// held processing (container recalculations, update processing,
	// etc)...
	my_thread->execute_batched_jobs();
}

///////////////////////////////////////////////////////////////////////////

void connection_threadObj::dispatch_execute_batched_jobs()
{
	connection_thread IN_THREAD{this};

	// Make sure all changes in the main execution thread are
	// committed by now. Although this should theoretically
	// taken care of by the mutex, this is technically required
	// for the connection thread to see what it needs to see.

	std::atomic_thread_fence(std::memory_order_acquire);

	process_events(batched_queue);
}

LIBCXXW_NAMESPACE_END
