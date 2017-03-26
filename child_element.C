/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "child_element.H"
#include "draw_info.H"
#include "draw_info_cache.H"
#include "connection_thread.H"
#include "container.H"
#include "layoutmanager.H"
#include "background_color.H"
#include "generic_window_handler.H"
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

child_elementObj::child_elementObj(const ref<containerObj::implObj> &container,
				   const metrics::horizvert_axi
				   &initial_metrics,
				   const std::string &scratch_buffer_id)
	: child_elementObj(container, initial_metrics, scratch_buffer_id,
			   background_colorptr())
{
}

child_elementObj::child_elementObj(const ref<containerObj::implObj> &container,
				   const metrics::horizvert_axi
				   &initial_metrics,
				   const std::string &scratch_buffer_id,
				   const background_colorptr
				   &initial_background_color)
	: elementObj::implObj(container->get_element_impl().nesting_level+1,
			      {0, 0, 0, 0},
			      // The container will position me later
			      initial_metrics,
			      container->get_window_handler().get_screen(),
			      container->get_window_handler().drawable_pictformat,
			      scratch_buffer_id),
	current_background_color_thread_only(initial_background_color),
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

draw_info &child_elementObj::get_draw_info(IN_THREAD_ONLY)
{
	auto &c=*IN_THREAD->current_draw_info_cache(IN_THREAD);
	auto e=ref<elementObj::implObj>(this);

	auto iter=c.draw_info_cache.find(e);

	if (iter != c.draw_info_cache.end())
		return iter->second;

	// Start by copying the parent to the child

	draw_info &di=
		c.draw_info_cache.insert({e,
					container->get_element_impl()
					.get_draw_info(IN_THREAD)}).first
		->second;

	// Add this parent's x/y coordinates to current_position, calculating
	// the new absolute_location
	auto cpy=data(IN_THREAD).current_position;

	cpy.x = coord_t::truncate(cpy.x+di.absolute_location.x);
	cpy.y = coord_t::truncate(cpy.y+di.absolute_location.y);

	// But before we update di.absolute_location, compute the intersect
	// between the parent's absolute_location and this element's
	// aboslute_location.
	//
	// If this element lies outside of the parent's absolute location
	// it is invisible.

	rectangle_set a{di.absolute_location};
	rectangle_set b{cpy};

	auto res=intersect(a,b);

	if (res.size() > 1)
		throw EXCEPTION("Unexpected result from an intersection of two rectangles");

	rectangle res_rect;

	if (res.empty())
	{
		// std::cout << "EMPTY VIEWPORT" << std::endl;
	}
	else
		res_rect=*res.begin();

	di.absolute_location=cpy;
	di.element_viewport=res_rect;

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

void child_elementObj::remove_background_color(IN_THREAD_ONLY)
{
	current_background_color(IN_THREAD)=nullptr;
	background_color_changed(IN_THREAD);
	container->child_background_color_changed(IN_THREAD,
						  ref<elementObj::implObj>
						  (this));
}

void child_elementObj::set_background_color(IN_THREAD_ONLY,
					    const background_color &bgcolor)
{
	current_background_color(IN_THREAD)=bgcolor;
	background_color_changed(IN_THREAD);
	container->child_background_color_changed(IN_THREAD,
						  ref<elementObj::implObj>
						  (this));
}

void child_elementObj::theme_updated(IN_THREAD_ONLY)
{
	if (!current_background_color(IN_THREAD).null())
		current_background_color(IN_THREAD)->theme_updated(IN_THREAD);

	elementObj::implObj::theme_updated(IN_THREAD);
}

void child_elementObj
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &visibility_info)
{
	elementObj::implObj::set_inherited_visibility(IN_THREAD,
						      visibility_info);
	container->child_visibility_changed(IN_THREAD,
					    visibility_info,
					    elementimpl(this));
}

bool child_elementObj::has_own_background_color(IN_THREAD_ONLY)
{
	return !current_background_color(IN_THREAD).null();
}

void child_elementObj::prepare_draw_info(IN_THREAD_ONLY, draw_info &di)
{
	if (current_background_color(IN_THREAD).null())
		return;

	if (!data(IN_THREAD).inherited_visibility)
		return; // None, use parent background.

	di.window_background=current_background_color(IN_THREAD)
		->get_current_color(IN_THREAD)->impl;
	di.background_x=di.absolute_location.x;
	di.background_y=di.absolute_location.y;
}

LIBCXXW_NAMESPACE_END
