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

LIBCXXW_NAMESPACE_START

layoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: container_impl(container_impl)
{
}

layoutmanagerObj::implObj::~implObj()=default;

void layoutmanagerObj::implObj::needs_recalculation()
{
	container_impl->get_window_handler().thread()
		->run_as(RUN_AS,
			 [me=ref<layoutmanagerObj::implObj>(this)]
			 (IN_THREAD_ONLY)
			 {
				 me->needs_recalculation(IN_THREAD);
			 });
}

void layoutmanagerObj::implObj::needs_recalculation(IN_THREAD_ONLY)
{
	recalculate_needed(IN_THREAD)=true;

	(*IN_THREAD->containers_2_recalculate(IN_THREAD))
		[container_impl->get_element_impl().nesting_level]
		.insert(ref<layoutmanagerObj::implObj>(this));
}

void layoutmanagerObj::implObj::check_if_recalculate_needed(IN_THREAD_ONLY)
{
	auto &flag=recalculate_needed(IN_THREAD);

	if (!flag)
		return;

	flag=false;
	recalculate(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
