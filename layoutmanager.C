/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "layoutmanager.H"
#include "container.H"
#include "xid_t.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "batch_queue.H"

LIBCXXW_NAMESPACE_START

layoutmanagerObj::layoutmanagerObj(const ref<implObj> &impl)
	: impl(impl),
	  queue(impl->container_impl->get_window_handler().thread()
		->get_batch_queue())

{
}

// When the public object drops off, trigger layout recalculation.

layoutmanagerObj::~layoutmanagerObj()
{
	impl->needs_recalculation(queue);
}

LIBCXXW_NAMESPACE_END
