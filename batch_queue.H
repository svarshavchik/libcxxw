/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef batch_queue_h
#define batch_queue_h

#include "x/w/batch_queuefwd.H"
#include "batch_queueobj.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

template<typename functor>
void batch_queueObj::run_as(functor &&f)
{
	my_thread->run_as_batched(std::forward<functor>(f));
}

LIBCXXW_NAMESPACE_END

#endif
