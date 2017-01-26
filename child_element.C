/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "child_element.H"
#include "draw_info.H"
#include "container.H"
#include "layoutmanager.H"
#include "x/w/picture.H"

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

	draw_info di=container->get_element_impl()
		.get_draw_info(IN_THREAD, revised_viewport);

	prepare_draw_info(IN_THREAD, di);

	return di;
}

void child_elementObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
	if (!container->get_element_impl()
	    .data(IN_THREAD).inherited_visibility)
		flag=false;

	elementObj::implObj::visibility_updated(IN_THREAD, flag);
}

// When metrics are updated, notify my layout manager.
void child_elementObj::horizvert_updated(IN_THREAD_ONLY)
{
	container->invoke_layoutmanager([&]
					(const auto &manager)
					{
						manager->child_metrics_updated
							(IN_THREAD);
					});
}

child_elementObj::update_background_color_t
child_elementObj::no_background()
{
	return [](IN_THREAD_ONLY, draw_info &, child_elementObj &)
	{
	};
}

void child_elementObj::remove_background_color(IN_THREAD_ONLY)
{
	background_color(IN_THREAD)=no_background();
	schedule_redraw_if_visible(IN_THREAD);
}

void child_elementObj::set_background_color(IN_THREAD_ONLY,
					    const const_picture &bgcolor)
{
	background_color(IN_THREAD)=
		[bgcolor](IN_THREAD_ONLY, draw_info &di, child_elementObj &e)
		{
			if (!e.data(IN_THREAD).inherited_visibility)
				return; // None, use parent background.

			di.window_background=bgcolor->impl;
			di.background_x=di.viewport.x;
			di.background_y=di.viewport.y;
		};
	schedule_redraw_if_visible(IN_THREAD);
}

void child_elementObj::prepare_draw_info(IN_THREAD_ONLY,
					 draw_info &di)
{
	background_color(IN_THREAD)(IN_THREAD, di, *this);
}

LIBCXXW_NAMESPACE_END
