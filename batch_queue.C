/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "batch_queue.H"
#include "connection_thread.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

class batch_queueObj::containers_2_recalculateObj : virtual public obj {

public:

	std::unordered_set<ref<layoutmanagerObj::implObj>> containers;

};

batch_queueObj::batch_queueObj(const connection_thread &my_thread)
	: c2r{ref<containers_2_recalculateObj>::create()},
	  my_thread(my_thread)
{
}

void batch_queueObj
::schedule_for_recalculation(const ref<layoutmanagerObj::implObj> &l)
{
	c2r_t::lock lock{c2r};

	(*lock)->containers.insert(l);
}

batch_queueObj::~batch_queueObj()
{
	c2r_t::lock lock{c2r};

	auto containers=*lock;

	if (!containers->containers.empty())
		run_as(RUN_AS,
		       [containers]
		       (IN_THREAD_ONLY)
		       {
			       for (const auto &c:containers->containers)
				       c->needs_recalculation(IN_THREAD);
		       });

	my_thread->execute_batched_jobs();
}

LIBCXXW_NAMESPACE_END
