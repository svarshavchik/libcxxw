/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "batch_queue.H"
#include "connection_thread.H"
#include "layoutmanager.H"
#include "container.H"
#include "element.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::batch_queueObj);

LIBCXXW_NAMESPACE_START

containers_2_batch_recalculate_set::containers_2_batch_recalculate_set()
=default;

containers_2_batch_recalculate_set::~containers_2_batch_recalculate_set()
=default;

elements_2_batch_showhide_map::elements_2_batch_showhide_map()=default;

elements_2_batch_showhide_map::~elements_2_batch_showhide_map()=default;

batch_queueObj::batch_queueObj(const connection_thread &my_thread)
	: something_scheduled{false}, my_thread(my_thread)
{
	LOG_DEBUG("Created " << this);
}

void batch_queueObj
::schedule_for_recalculation(const ref<layoutmanagerObj::implObj> &l)
{
	my_thread->run_as([c=l->container_impl]
			  (ONLY IN_THREAD)
			  {
				  IN_THREAD->containers_2_batch_recalculate
					  (IN_THREAD)
					  ->containers.insert(c);
			  });

	something_scheduled=true;
}

void batch_queueObj
::schedule_for_visibility(const ref<elementObj::implObj> &e,
			  bool visibility)
{
	my_thread->run_as([=]
			  (ONLY IN_THREAD)
			  {
				  IN_THREAD->elements_2_batch_showhide
					  (IN_THREAD)->elements[e]=visibility;
			  });

	something_scheduled=true;
}



batch_queueObj::~batch_queueObj()
{
	LOG_DEBUG("Destroyed " << this);
	if (something_scheduled.get())
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

	// Empty out the containers_2_batch_recalculate and
	// elements_2_batch_showhide, then process
	// the batch queue.

	auto &containers=containers_2_batch_recalculate(IN_THREAD)->containers;

	for (const auto &c:containers)
		c->needs_recalculation(IN_THREAD);
	containers.clear();

	auto &elements=elements_2_batch_showhide(IN_THREAD)->elements;

	for (const auto &e:elements)
		e.first->request_visibility(IN_THREAD, e.second);
	elements.clear();

	process_events(batched_queue);
}

LIBCXXW_NAMESPACE_END
