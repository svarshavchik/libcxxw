/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_thread.H"
#include "connection_info.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

new_xid::new_xid(const connection_thread &thread_)
	: thread_(thread_), id_(thread_->info->alloc_xid())
{
}

new_xid::~new_xid()
{
	thread_->info->release_xid(id_);
}

LIBCXXW_NAMESPACE_END
