/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "layoutmanager.H"
#include "container.H"
#include "generic_window_handler.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "draw_info.H"
#include "child_element.H"
#include "catch_exceptions.H"
#include "batch_queue.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::layoutmanagerObj::implObj);

LIBCXXW_NAMESPACE_START

layoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

layoutmanagerObj::implObj::~implObj()=default;

void layoutmanagerObj::implObj::needs_recalculation(const batch_queue &queue)
{
	queue->run_as(RUN_AS,
		      [me=ref<layoutmanagerObj::implObj>(this)]
		      (IN_THREAD_ONLY)
		      {
			      me->needs_recalculation(IN_THREAD);
		      });
}

void layoutmanagerObj::implObj::child_metrics_updated(IN_THREAD_ONLY)
{
	needs_recalculation(IN_THREAD);
}

void layoutmanagerObj::implObj::needs_recalculation(IN_THREAD_ONLY)
{
	(*IN_THREAD->containers_2_recalculate(IN_THREAD))
		[container_impl->get_element_impl().nesting_level]
		.insert(container_impl);
}

void layoutmanagerObj::implObj::current_position_updated(IN_THREAD_ONLY)
{
	container_impl->get_element_impl().current_position_updated(IN_THREAD);
}

void layoutmanagerObj::implObj
::child_background_color_changed(IN_THREAD_ONLY,
				 const ref<elementObj::implObj> &child)
{
}

LIBCXXW_NAMESPACE_END
