/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element_position_updated.H"
#include "x/w/impl/element.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container.H"
#include "generic_window_handler.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

void elementObj::implObj::schedule_update_position_processing(ONLY IN_THREAD)
{
	IN_THREAD->element_position_updated(IN_THREAD)
		->set(IN_THREAD)[nesting_level].insert(ref{this});
}

bool elementObj::implObj::update_position_processing_scheduled(ONLY IN_THREAD)
{
	auto &set=
		IN_THREAD->element_position_updated(IN_THREAD)->set(IN_THREAD);

	auto level_iter=set.find(nesting_level);

	if (level_iter == set.end())
		return false;

	return level_iter->second.find(ref{this})
		!= level_iter->second.end();
}

elementObj::implObj *child_elementObj::get_parent_element_impl() const
{
	return &child_container->container_element_impl();
}

elementObj::implObj *generic_windowObj::handlerObj::get_parent_element_impl()
	const
{
	return nullptr;
}

LIBCXXW_NAMESPACE_END
