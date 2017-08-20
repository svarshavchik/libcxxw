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
#include "x/w/motion_event.H"

LIBCXXW_NAMESPACE_START

child_elementObj::child_elementObj(const ref<containerObj::implObj> &child_container)
	: child_elementObj(child_container, {})
{
}

child_elementObj::child_elementObj(const ref<containerObj::implObj> &child_container,
				   const child_element_init_params &init_params)
	: child_elementObj(child_container, init_params, background_colorptr())
{
}

child_elementObj::child_elementObj(const ref<containerObj::implObj> &child_container,
				   const child_element_init_params &init_params,
				   const background_colorptr
				   &initial_background_color)
	: elementObj::implObj(child_container->get_element_impl()
			      .nesting_level+1,
			      {0, 0, 0, 0},
			      // The container will position me later
			      init_params.initial_metrics,
			      child_container->get_window_handler()
			      .get_screen(),
			      child_container->get_window_handler()
			      .drawable_pictformat,
			      init_params.scratch_buffer_id.empty()
			      ? "default@libcxx":init_params.scratch_buffer_id),
	current_background_color_thread_only(initial_background_color),
	child_container(init_params.container_override ? child_container
		  : child_container->parent_for_new_child(child_container))
{
}

child_elementObj::~child_elementObj()=default;

generic_windowObj::handlerObj &child_elementObj::get_window_handler()
{
	return child_container->get_window_handler();
}

const generic_windowObj::handlerObj &child_elementObj::get_window_handler()
	const
{
	return child_container->get_window_handler();
}

void child_elementObj::process_updated_position(IN_THREAD_ONLY)
{
	elementObj::implObj::process_updated_position(IN_THREAD);
	child_container->get_element_impl().schedule_redraw(IN_THREAD);
}

draw_info &child_elementObj::get_draw_info(IN_THREAD_ONLY)
{
	auto &c=*IN_THREAD->current_draw_info_cache(IN_THREAD);
	auto e=ref<elementObj::implObj>(this);

	auto iter=c.draw_info_cache.find(e);

	if (iter != c.draw_info_cache.end())
	{
		// Between the the time schedule_redraw() was called, we
		// could've been removed from my container. Recalculation
		// has higher priority than drawing, so we should no longer
		// scribble over our window after we've been removed.

		if (data(IN_THREAD).removed)
			iter->second.element_viewport.clear();
		return iter->second;
	}

	// Start by copying the parent to the child

	draw_info &di=
		c.draw_info_cache.insert({e,
					child_container->get_element_impl()
					.get_draw_info(IN_THREAD)}).first
		->second;

	// Add this parent's x/y coordinates to current_position, calculating
	// the new absolute_location
	auto cpy=data(IN_THREAD).current_position;

	cpy.x = coord_t::truncate(cpy.x+di.absolute_location.x);
	cpy.y = coord_t::truncate(cpy.y+di.absolute_location.y);

	// But before we update di.absolute_location, compute the intersect
	// between the parent's viewport and this element's absolute_location,
	// to derive this element's viewport.

	di.element_viewport=intersect(di.element_viewport, {cpy});

	if (data(IN_THREAD).removed)
		di.element_viewport.clear(); // See above.

	di.absolute_location=cpy;

	prepare_draw_info(IN_THREAD, di);

	return di;
}

rectangle child_elementObj::get_absolute_location(IN_THREAD_ONLY)
{
	auto r=child_container->get_element_impl().get_absolute_location(IN_THREAD);

	auto cpy=data(IN_THREAD).current_position;

	cpy.x = coord_t::truncate(cpy.x+r.x);
	cpy.y = coord_t::truncate(cpy.y+r.y);
	return cpy;
}

void child_elementObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
	if (!child_container->get_element_impl()
	    .data(IN_THREAD).inherited_visibility)
		flag=false;

	elementObj::implObj::visibility_updated(IN_THREAD, flag);
}

// When metrics are updated, notify my layout manager.
void child_elementObj::horizvert_updated(IN_THREAD_ONLY)
{
	child_container->invoke_layoutmanager([&]
					(const auto &manager)
					{
						manager->child_metrics_updated
							(IN_THREAD);
					});
}

void child_elementObj::initialize(IN_THREAD_ONLY)
{
	elementObj::implObj::initialize(IN_THREAD);
	if (current_background_color(IN_THREAD))
		current_background_color(IN_THREAD)->initialize(IN_THREAD);
}

void child_elementObj::theme_updated(IN_THREAD_ONLY,
				     const defaulttheme &new_theme)
{
	elementObj::implObj::theme_updated(IN_THREAD, new_theme);
	if (current_background_color(IN_THREAD))
		current_background_color(IN_THREAD)->theme_updated(IN_THREAD,
								   new_theme);
}

void child_elementObj::remove_background_color(IN_THREAD_ONLY)
{
	if (!current_background_color(IN_THREAD))
		return; // no-op

	current_background_color(IN_THREAD)=nullptr;
	background_color_changed(IN_THREAD);
	child_container->child_background_color_changed(IN_THREAD,
						  ref<elementObj::implObj>
						  (this));
}

void child_elementObj::set_background_color(IN_THREAD_ONLY,
					    const background_color &bgcolor)
{
	if (current_background_color(IN_THREAD) == bgcolor)
		return; // noop

	current_background_color(IN_THREAD)=bgcolor;
	background_color_changed(IN_THREAD);
	child_container->child_background_color_changed(IN_THREAD,
						  ref<elementObj::implObj>
						  (this));
}

void child_elementObj
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &visibility_info)
{
	bool current_flag=data(IN_THREAD).inherited_visibility;
	bool new_flag=visibility_info.flag;

	elementObj::implObj::set_inherited_visibility(IN_THREAD,
						      visibility_info);

	if (current_flag != new_flag)
		child_container->child_visibility_updated(IN_THREAD,
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

void child_elementObj::window_focus_change(IN_THREAD_ONLY, bool flag)
{
	elementObj::implObj::window_focus_change(IN_THREAD, flag);
	child_container->get_element_impl().window_focus_change(IN_THREAD, flag);
}

bool child_elementObj::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	return elementObj::implObj::process_key_event(IN_THREAD, ke)
		|| child_container->get_element_impl()
		.process_key_event(IN_THREAD, ke);
}

bool child_elementObj::enabled(IN_THREAD_ONLY)
{
	if (!child_container->get_element_impl().enabled(IN_THREAD))
		return false;

	return elementObj::implObj::enabled(IN_THREAD);
}

bool child_elementObj::process_button_event(IN_THREAD_ONLY,
					    const button_event &be,
					    xcb_timestamp_t timestamp)
{
	auto ret=elementObj::implObj::process_button_event(IN_THREAD, be,
							   timestamp);

	if (child_container->get_element_impl()
	    .process_button_event(IN_THREAD, be, timestamp))
		ret=true;

	return ret;
}

void child_elementObj::grab(IN_THREAD_ONLY)
{
	get_window_handler().grab(IN_THREAD, ref<elementObj::implObj>(this));
}

void child_elementObj::report_motion_event(IN_THREAD_ONLY,
					   const motion_event &me)
{
	elementObj::implObj::report_motion_event(IN_THREAD, me);

	auto cpy=me;

	cpy.x=coord_t::truncate(me.x + data(IN_THREAD).current_position.x);
	cpy.y=coord_t::truncate(me.y + data(IN_THREAD).current_position.y);

	child_container->get_element_impl().report_motion_event(IN_THREAD, cpy);
}

void child_elementObj::ensure_visibility(IN_THREAD_ONLY, const rectangle &r)
{
	child_container->ensure_visibility(IN_THREAD, *this, r);
}

bool child_elementObj::pasted(IN_THREAD_ONLY,
			      const std::u32string_view &str)
{
	return elementObj::implObj::pasted(IN_THREAD, str) ||
		child_container->get_element_impl().pasted(IN_THREAD, str);
}

void child_elementObj::focusable_initialized(IN_THREAD_ONLY,
					     focusableImplObj &fimpl)
{
	child_container->get_element_impl().focusable_initialized(IN_THREAD,
								  fimpl);
}

void child_elementObj::get_focus_first(IN_THREAD_ONLY, const focusable &f)
{
	child_container->get_element_impl().get_focus_first(IN_THREAD, f);
}

void child_elementObj::creating_focusable_element()
{
	return child_container->get_element_impl().creating_focusable_element();
}

const char *child_elementObj::label_theme_font() const
{
	return child_container->get_element_impl().label_theme_font();
}

void child_elementObj::schedule_tooltip_creation(IN_THREAD_ONLY)
{
	elementObj::implObj::schedule_tooltip_creation(IN_THREAD);
	child_container->get_element_impl().schedule_tooltip_creation(IN_THREAD);
}

void child_elementObj::unschedule_tooltip_creation(IN_THREAD_ONLY)
{
	elementObj::implObj::unschedule_tooltip_creation(IN_THREAD);
	child_container->get_element_impl().unschedule_tooltip_creation(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
