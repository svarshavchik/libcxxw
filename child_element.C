/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/child_element.H"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/container.H"
#include "connection_thread.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/background_color.H"
#include "generic_window_handler.H"
#include "x/w/impl/background_color_element.H"
#include "cursor_pointer.H"
#include "inherited_visibility_info.H"
#include "screen.H"
#include "catch_exceptions.H"
#include "x/w/picture.H"
#include "x/w/motion_event.H"
#include "x/w/rgb.H"

LIBCXXW_NAMESPACE_START

child_elementObj::child_elementObj(const container_impl &child_container)
	: child_elementObj(child_container, {})
{
}

child_elementObj::child_elementObj(const container_impl &child_container,
				   const child_element_init_params &init_params)
	: superclass_t{child_container->container_element_impl()

		// If no background color is given, create a placeholder
		// black rgb color. is_mine_background_color will specify
		// what the deal is.
		       .create_background_color(init_params.background_color
						? *init_params.background_color
						: color_arg{rgb{}}),
		       child_container->container_element_impl()
		       .nesting_level+1,
		       rectangle{0, 0, 0, 0},
		       init_params.attached_popup,
		// The container will position me later
		       init_params.initial_metrics,
		       child_container->get_window_handler()
		       .get_screen(),
		       child_container->get_window_handler()
		       .drawable_pictformat,
		       (init_params.scratch_buffer_id.empty()
			? "default@libcxx.com":init_params.scratch_buffer_id)},
	  is_mine_background_color{init_params.background_color ? true:false},
	  child_container(child_container)
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

void child_elementObj::process_updated_position(ONLY IN_THREAD)
{
	superclass_t::process_updated_position(IN_THREAD);
	child_container->container_element_impl()
		.schedule_full_redraw(IN_THREAD);
}

draw_info &child_elementObj::get_draw_info(ONLY IN_THREAD)
{
	if (data(IN_THREAD).cached_draw_info)
	{
		auto &di=*data(IN_THREAD).cached_draw_info;

		// Between the the time schedule_full_redraw() was called, we
		// could've been removed from my container. Recalculation
		// has higher priority than drawing, so we should no longer
		// scribble over our window after we've been removed.

		if (data(IN_THREAD).removed)
			di.element_viewport.clear();
		return di;
	}

	return get_draw_info_from_scratch(IN_THREAD);
}

draw_info &child_elementObj::get_draw_info_from_scratch(ONLY IN_THREAD)
{
	// Start by copying the parent to the child

	// This means that if an element has a cached_draw_info, the parent
	// container should have one too.

	data(IN_THREAD).cached_draw_info=child_container
		->container_element_impl().get_draw_info(IN_THREAD);

	draw_info &di=*data(IN_THREAD).cached_draw_info;

	// Add this parent's x/y coordinates to current_position, calculating
	// the new absolute_location
	auto cpy=data(IN_THREAD).current_position;

	cpy.x = coord_t::truncate(cpy.x+di.absolute_location.x);
	cpy.y = coord_t::truncate(cpy.y+di.absolute_location.y);

	// But before we update di.absolute_location, compute the intersect
	// between the parent's viewport and this element's absolute_location,
	// to derive this element's viewport.

	di.element_viewport=intersect(di.element_viewport, cpy);

	if (data(IN_THREAD).removed)
		di.element_viewport.clear(); // See above.

	di.absolute_location=cpy;

	// Figure out the background color.
	//
	// See also: update_draw_info_background_color().

	if (has_own_background_color(IN_THREAD) &&
	    data(IN_THREAD).logical_inherited_visibility)
		// If we're not visible, we use the parent background
	{
		auto bg=background_color_element_implObj::get(IN_THREAD);
		di.window_background_color=bg->get_current_color(IN_THREAD);
		di.background_x=di.absolute_location.x;
		di.background_y=di.absolute_location.y;
	}

	return di;
}

void child_elementObj
::update_draw_info_background_color(ONLY IN_THREAD)
{
	if (!data(IN_THREAD).cached_draw_info)
		return; // We don't have anything cached.

	auto &di=*data(IN_THREAD).cached_draw_info;

	auto &parent_di=child_container
		->container_element_impl().get_draw_info(IN_THREAD);

	di.window_background_color=parent_di.window_background_color;
	di.background_x=parent_di.background_x;
	di.background_y=parent_di.background_y;

	if (has_own_background_color(IN_THREAD) &&
	    data(IN_THREAD).logical_inherited_visibility)
		// If we're not visible, we use the parent background
	{
		auto bg=background_color_element_implObj::get(IN_THREAD);
		di.window_background_color=bg->get_current_color(IN_THREAD);
		di.background_x=di.absolute_location.x;
		di.background_y=di.absolute_location.y;
	}
}

void child_elementObj::background_color_changed(ONLY IN_THREAD)
{
	update_draw_info_background_color(IN_THREAD);
	superclass_t::background_color_changed(IN_THREAD);
}

rectangle child_elementObj::get_absolute_location(ONLY IN_THREAD)
	const
{
	auto r=child_container->container_element_impl()
		.get_absolute_location(IN_THREAD);

	auto cpy=data(IN_THREAD).current_position;

	cpy.x = coord_t::truncate(cpy.x+r.x);
	cpy.y = coord_t::truncate(cpy.y+r.y);
	return cpy;
}

void child_elementObj::visibility_updated(ONLY IN_THREAD, bool flag)
{
	if (!child_container->container_element_impl()
	    .data(IN_THREAD).logical_inherited_visibility)
		flag=false;

	superclass_t::visibility_updated(IN_THREAD, flag);
}

// When metrics are updated, notify my layout manager.
void child_elementObj::horizvert_updated(ONLY IN_THREAD)
{
	if (data(IN_THREAD).metrics_update_callback)
		try {
			auto hv=get_horizvert(IN_THREAD);

			data(IN_THREAD).metrics_update_callback
				(IN_THREAD, hv->horiz, hv->vert);
		} REPORT_EXCEPTIONS(this);

	child_container->invoke_layoutmanager([&]
					(const auto &manager)
					{
						manager->child_metrics_updated
							(IN_THREAD);
					});
}

void child_elementObj::remove_background_color(ONLY IN_THREAD)
{
	if (!is_mine_background_color)
		return; // no-op

	is_mine_background_color=false;
	background_color_changed(IN_THREAD);
	child_container->child_background_color_changed(IN_THREAD,
						  ref<elementObj::implObj>
						  (this));
}

background_color child_elementObj::current_background_color(ONLY IN_THREAD)
{
	if (has_own_background_color(IN_THREAD))
		return background_color_element_implObj::get(IN_THREAD);

	return child_container->container_element_impl()
		.current_background_color(IN_THREAD);
}

void child_elementObj::set_background_color(ONLY IN_THREAD,
					    const background_color &bgcolor)
{
	// The background color may be a gradient that gets adjusted for
	// the current display element's size.
	//
	// For this reason we check if the background color has changed,
	// in the following manner, AFTER the update().

	background_colorptr old_color;

	if (is_mine_background_color)
		old_color=background_color_element_implObj::get(IN_THREAD);

	is_mine_background_color=true;
	background_color_element<child_element_bgcolor>
		::update(IN_THREAD, bgcolor);

	if (background_color_element_implObj::get(IN_THREAD) == old_color)
		return; // noop

	// Ok, now the background color has changed (1/2).
	background_color_changed(IN_THREAD);
	child_container->child_background_color_changed(IN_THREAD,
						  ref<elementObj::implObj>
						  (this));
}

void child_elementObj::theme_updated(ONLY IN_THREAD,
				     const const_defaulttheme &new_theme)
{
	if (!is_mine_background_color)
	{
		superclass_t::theme_updated(IN_THREAD, new_theme);
		return;
	}

	// Check if the theme changes our background color.

	auto b=background_color_element_implObj::get(IN_THREAD);

	superclass_t::theme_updated(IN_THREAD, new_theme);

	if (b == background_color_element_implObj::get(IN_THREAD))
		return;

	// Ok, now the background color has changed (2/2).

	background_color_changed(IN_THREAD);
	child_container->child_background_color_changed
		(IN_THREAD,
		 ref<elementObj::implObj>(this));
}

void child_elementObj::request_visibility(ONLY IN_THREAD, bool flag)
{
	bool current_flag=data(IN_THREAD).requested_visibility;

	superclass_t::request_visibility(IN_THREAD, flag);

	if (current_flag != data(IN_THREAD).requested_visibility)
		child_container->requested_child_visibility_updated
			(IN_THREAD, element_impl(this),
			 data(IN_THREAD).requested_visibility);
}

void child_elementObj
::set_inherited_visibility(ONLY IN_THREAD,
			   inherited_visibility_info &visibility_info)
{
	bool current_flag=data(IN_THREAD).logical_inherited_visibility;
	bool new_flag=visibility_info.flag;

	superclass_t::set_inherited_visibility(IN_THREAD,
					       visibility_info);

	if (current_flag != new_flag)
	{
		update_draw_info_background_color(IN_THREAD);
		child_container->inherited_child_visibility_updated
			(IN_THREAD, element_impl(this),
			 visibility_info);
	}
}

bool child_elementObj::has_own_background_color(ONLY IN_THREAD)
{
	return is_mine_background_color;
}

void child_elementObj::window_focus_change(ONLY IN_THREAD, bool flag)
{
	superclass_t::window_focus_change(IN_THREAD, flag);
	child_container->container_element_impl()
		.window_focus_change(IN_THREAD, flag);
}

bool child_elementObj::process_key_event(ONLY IN_THREAD, const key_event &ke)
{
	return superclass_t::process_key_event(IN_THREAD, ke)
		|| child_container->container_element_impl()
		.process_key_event(IN_THREAD, ke);
}

bool child_elementObj::enabled(ONLY IN_THREAD)
{
	if (!child_container->container_element_impl().enabled(IN_THREAD))
		return false;

	return superclass_t::enabled(IN_THREAD);
}

bool child_elementObj::draw_to_window_picture_as_disabled(ONLY IN_THREAD)
{
	return superclass_t::draw_to_window_picture_as_disabled(IN_THREAD)
		|| child_container->container_element_impl()
		.draw_to_window_picture_as_disabled(IN_THREAD);
}

bool child_elementObj::process_button_event(ONLY IN_THREAD,
					    const button_event &be,
					    xcb_timestamp_t timestamp)
{
	auto ret=superclass_t::process_button_event(IN_THREAD, be,
							   timestamp);

	if (child_container->container_element_impl()
	    .process_button_event(IN_THREAD, be, timestamp))
		ret=true;

	return ret;
}

void child_elementObj::grab(ONLY IN_THREAD)
{
	get_window_handler().grab(IN_THREAD, element_impl{this});
}

void child_elementObj::report_motion_event(ONLY IN_THREAD,
					   const motion_event &me)
{
	superclass_t::report_motion_event(IN_THREAD, me);

	auto cpy=me;

	cpy.x=coord_t::truncate(me.x + data(IN_THREAD).current_position.x);
	cpy.y=coord_t::truncate(me.y + data(IN_THREAD).current_position.y);

	child_container->container_element_impl()
		.report_motion_event(IN_THREAD, cpy);
}

void child_elementObj::ensure_visibility(ONLY IN_THREAD, const rectangle &r)
{
	child_container->ensure_visibility(IN_THREAD, *this, r);
}

bool child_elementObj::pasted(ONLY IN_THREAD,
			      const std::u32string_view &str)
{
	return superclass_t::pasted(IN_THREAD, str) ||
		child_container->container_element_impl()
		.pasted(IN_THREAD, str);
}

void child_elementObj::focusable_initialized(ONLY IN_THREAD,
					     focusableObj::implObj &fimpl)
{
	child_container->container_element_impl()
		.focusable_initialized(IN_THREAD,
				       fimpl);
}

void child_elementObj::get_focus_first(ONLY IN_THREAD, const focusable &f)
{
	child_container->container_element_impl()
		.get_focus_first(IN_THREAD, f);
}

void child_elementObj::creating_focusable_element()
{
	return child_container->container_element_impl()
		.creating_focusable_element();
}

font_arg child_elementObj::label_theme_font() const
{
	return child_container->container_element_impl().label_theme_font();
}

color_arg child_elementObj::label_theme_color() const
{
	return child_container->container_element_impl().label_theme_color();
}

void child_elementObj::schedule_hover_action(ONLY IN_THREAD)
{
	superclass_t::schedule_hover_action(IN_THREAD);
	child_container->container_element_impl()
		.schedule_hover_action(IN_THREAD);
}

void child_elementObj::unschedule_hover_action(ONLY IN_THREAD)
{
	superclass_t::unschedule_hover_action(IN_THREAD);
	child_container->container_element_impl()
		.unschedule_hover_action(IN_THREAD);
}

cursor_pointerptr child_elementObj::get_cursor_pointer(ONLY IN_THREAD)
{
	auto p=superclass_t::get_cursor_pointer(IN_THREAD);

	if (!p)
		p=child_container->container_element_impl()
			.get_cursor_pointer(IN_THREAD);

	return p;
}

void child_elementObj::element_name(std::ostream &o)
{
	child_container->container_element_impl().element_name(o);
	o << "/";
	superclass_t::element_name(o);
}

LIBCXXW_NAMESPACE_END
