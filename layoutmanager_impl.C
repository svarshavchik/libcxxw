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
	queue->schedule_for_recalculation(ref<implObj>(this));
}

void layoutmanagerObj::implObj::child_metrics_updated(IN_THREAD_ONLY)
{
	needs_recalculation(IN_THREAD);
}

void layoutmanagerObj::implObj::needs_recalculation(IN_THREAD_ONLY)
{
	container_impl->needs_recalculation(IN_THREAD);
}

void layoutmanagerObj::implObj::current_position_updated(IN_THREAD_ONLY)
{
	container_impl->get_element_impl().current_position_updated(IN_THREAD);
}

void layoutmanagerObj::implObj
::child_background_color_changed(IN_THREAD_ONLY, const elementimpl &child)
{
}

void layoutmanagerObj::implObj
::child_visibility_changed(IN_THREAD_ONLY,
			   inherited_visibility_info &info,
			   const elementimpl &child)
{
}

rectangle layoutmanagerObj::implObj::padded_position(IN_THREAD_ONLY,
						     const elementimpl &e_impl)
{
	return e_impl->data(IN_THREAD).current_position;
}

void layoutmanagerObj::implObj::theme_updated(IN_THREAD_ONLY)
{
}

void layoutmanagerObj::implObj::ensure_visibility(IN_THREAD_ONLY,
						  elementObj::implObj &e,
						  const rectangle &r)
{
}

LIBCXXW_NAMESPACE_END
