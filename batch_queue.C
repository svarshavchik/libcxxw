/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "batch_queue.H"
#include "connection_thread.H"
#include "layoutmanager.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

containers_2_batch_recalculate_set::containers_2_batch_recalculate_set()
=default;

containers_2_batch_recalculate_set::~containers_2_batch_recalculate_set()
=default;

batch_queueObj::batch_queueObj(const connection_thread &my_thread)
	: something_scheduled{false}, my_thread(my_thread)
{
}

void batch_queueObj
::schedule_for_recalculation(const ref<layoutmanagerObj::implObj> &l)
{
	my_thread->run_as([c=l->container_impl]
			  (IN_THREAD_ONLY)
			  {
				  IN_THREAD->containers_2_batch_recalculate
					  (IN_THREAD)
					  ->containers.insert(c);
			  });

	*mpobj<bool>::lock{something_scheduled}=true;
}

batch_queueObj::~batch_queueObj()
{
	if (*mpobj<bool>::lock{something_scheduled})
		my_thread->execute_batched_jobs();
}

///////////////////////////////////////////////////////////////////////////

void connection_threadObj::dispatch_execute_batched_jobs()
{
	const connection_thread thread_(this);

	// Make sure all changes in the main execution thread are
	// committed by now. Although this should theoretically
	// taken care of by the mutex, this is technically required
	// for the connection thread to see what it needs to see.

	std::atomic_thread_fence(std::memory_order_acquire);

	// Empty out the containers_2_batch_recalculate, then process
	// the batch queue.

	auto &containers=containers_2_batch_recalculate(IN_THREAD)->containers;

	for (const auto &c:containers)
		c->needs_recalculation(IN_THREAD);
	containers.clear();
	process_events(batched_queue);
}

LIBCXXW_NAMESPACE_END
