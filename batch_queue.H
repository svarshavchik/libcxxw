/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef batch_queue_h
#define batch_queue_h

#include "x/w/batch_queuefwd.H"
#include "batch_queueobj.H"
#include "connection_thread.H"
#include "x/w/containerobj.H"

#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! Queue of containers to recalculate in batch.

class LIBCXX_HIDDEN containers_2_batch_recalculate_set {

public:

	//! Constructor
	containers_2_batch_recalculate_set();

	//! Destructor
	~containers_2_batch_recalculate_set();
	//! The containers
	std::unordered_set<ref<containerObj::implObj>> containers;
};

template<typename functor>
void batch_queueObj::run_as(functor &&f)
{
	my_thread->run_as_batched(std::forward<functor>(f));

	*mpobj<bool>::lock{something_scheduled}=true;
}

LIBCXXW_NAMESPACE_END

#endif
