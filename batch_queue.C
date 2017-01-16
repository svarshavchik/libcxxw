/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "batch_queue.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

batch_queueObj::batch_queueObj(const connection_thread &my_thread)
	: my_thread(my_thread)
{
}

batch_queueObj::~batch_queueObj()
{
	my_thread->execute_batched_jobs();
}

LIBCXXW_NAMESPACE_END
