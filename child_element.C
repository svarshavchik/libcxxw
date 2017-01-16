/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "child_element.H"
#include "draw_info.H"
#include "container.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

child_elementObj::child_elementObj(const ref<containerObj::implObj> &container,
				   const metrics::axis &horiz,
				   const metrics::axis &vert)
	: elementObj::implObj(container->get_element_impl().nesting_level+1,
			      {0, 0, 0, 0},
			      // The container will position me later
			      horiz, vert),
	container(container)
{
}

child_elementObj::~child_elementObj()=default;

generic_windowObj::handlerObj &child_elementObj::get_window_handler()
{
	return container->get_window_handler();
}

const generic_windowObj::handlerObj &child_elementObj::get_window_handler()
	const
{
	return container->get_window_handler();
}

draw_info child_elementObj::get_draw_info(IN_THREAD_ONLY,
					  const rectangle &initial_viewport)
{
	auto revised_viewport=initial_viewport;

	auto &parent_position=container->get_element_impl()
		.data(IN_THREAD).current_position;

	revised_viewport.x += (coord_t::value_type)parent_position.x;
	revised_viewport.y += (coord_t::value_type)parent_position.y;

	return container->get_element_impl()
		.get_draw_info(IN_THREAD, revised_viewport);
}

// When metrics are updated, notify my layout manager.
void child_elementObj::horizvert_updated(IN_THREAD_ONLY)
{
	container->invoke_layoutmanager([&]
					(const auto &manager)
					{
						manager->needs_recalculation
							(IN_THREAD);
					});
}

LIBCXXW_NAMESPACE_END
