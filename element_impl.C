/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element.H"
#include "element_draw.H"
#include "screen.H"
#include "connection_thread.H"
#include "batch_queue.H"
#include "generic_window_handler.H"
#include "draw_info.H"
#include "busy.H"
#include "icon.H"
#include "icon_image.H"
#include "cursor_pointer.H"
#include "run_as.H"
#include "background_color.H"
#include "grabbed_pointer.H"
#include "x/w/element_state.H"
#include "x/w/scratch_buffer.H"
#include "x/w/motion_event.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/tooltip.H"
#include "x/callback_list.H"
#include "element_screen.H"
#include "focus/label_for.H"
#include "fonts/current_fontcollection.H"
#include "xim/ximclient.H"
#include "popup/popup.H"
#include "catch_exceptions.H"
#include <x/logger.H>
#include <x/weakcapture.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::elementObj::implObj);

LIBCXXW_NAMESPACE_START

// #define DEBUG_EXPOSURE_CALCULATIONS

#define DO_NOT_DRAW (!data(IN_THREAD).inherited_visibility)

void elementObj::implObj::data_thread_only_t::no_focus_callback(focus_change)
{
}

void elementObj::implObj::data_thread_only_t
::no_element_state_callback(const element_state &,
			    const busy &)
{
}

bool elementObj::implObj::data_thread_only_t
::no_key_event_callback(const all_key_events_t &,
			const busy &)
{
	return false;
}

elementObj::implObj::implObj(size_t nesting_level,
			     const rectangle &initial_position,
			     const screen &my_screen,
			     const const_pictformat &my_pictformat,
			     const std::string &scratch_buffer_id)
	: data_thread_only
	  ({
	      initial_position
	  }),
	  nesting_level(nesting_level),
	  element_scratch_buffer(my_screen->impl->create_scratch_buffer
				 (my_screen, scratch_buffer_id, my_pictformat,
				  initial_position.width / 20 + 1,
				  initial_position.height / 20 + 1))
{
}

elementObj::implObj::implObj(size_t nesting_level,
			     const rectangle &initial_position,
			     const metrics::horizvert_axi &initial_metrics,
			     const screen &my_screen,
			     const const_pictformat &my_pictformat,
			     const std::string &scratch_buffer_id)
	: metrics::horizvertObj(initial_metrics),
	data_thread_only
	({
		initial_position,
	}),
	nesting_level(nesting_level),
	element_scratch_buffer(my_screen->impl->create_scratch_buffer
			       (my_screen, scratch_buffer_id, my_pictformat,
				initial_position.width / 20 + 1,
				initial_position.height / 20 + 1))
{
}

elementObj::implObj::~implObj()=default;

void elementObj::implObj::removed_from_container(IN_THREAD_ONLY)
{
	// Who knows, maybe we haven't bee initialized yet?

	initialize_if_needed(IN_THREAD);

	if (data(IN_THREAD).removed)
		return;

	data(IN_THREAD).removed=true;
	removed(IN_THREAD);
	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->removed_from_container(IN_THREAD);
		       });
	set_inherited_visibility_flag(IN_THREAD, false);
}

void elementObj::implObj::removed(IN_THREAD_ONLY)
{
}

void elementObj::implObj::toggle_visibility()
{
	THREAD->get_batch_queue()
		->run_as([me=ref(this)]
			 (IN_THREAD_ONLY)
			 {
				 me->toggle_visibility(IN_THREAD);
			 });
}

void elementObj::implObj::request_visibility(bool flag)
{
	// Set requested_visibility, make sure this is done in the connection
	// thread.

	// Sets requested_visibility, then adds the element to the
	// visibility_updated list.

	// The connection thread invokes update_visibility after processing
	// all messages.

	THREAD->get_batch_queue()->run_as
		([flag, me=elementimpl(this)]
		 (IN_THREAD_ONLY)
		 {
			 me->request_visibility(IN_THREAD, flag);
		 });
}

void elementObj::implObj::toggle_visibility(IN_THREAD_ONLY)
{
	request_visibility(IN_THREAD,
			   !data(IN_THREAD).requested_visibility);
}

void elementObj::implObj::request_visibility(IN_THREAD_ONLY, bool flag)
{
	if (data(IN_THREAD).requested_visibility == flag)
		return;

	data(IN_THREAD).requested_visibility=flag;

	schedule_update_visibility(IN_THREAD);
}

void elementObj::implObj::schedule_update_visibility(IN_THREAD_ONLY)
{
	IN_THREAD->insert_element_set
		(*IN_THREAD->visibility_updated(IN_THREAD), elementimpl(this));
}

void elementObj::implObj::request_visibility_recursive(bool flag)
{
	THREAD->get_batch_queue()->run_as
		([flag, me=elementimpl(this)]
		 (IN_THREAD_ONLY)
		 {
			 me->request_visibility_recursive(IN_THREAD, flag);
		 });
}

void elementObj::implObj::request_visibility_recursive(IN_THREAD_ONLY,
						       bool flag)
{
	request_visibility(IN_THREAD, flag);
}

void elementObj::implObj::update_visibility(IN_THREAD_ONLY)
{
	// Ignore visibility updates until such time we are
	// initialize_if_needed().

	if (!initialized(IN_THREAD))
		return;

	// This is invoked from the connection thread, when it processes the
	// IN_THREAD->visibility_updated set.
	//
	// Do nothing if request_visibility() matches actual_visibility.
	// Otherwise set actual_visibility to requested_visibility and call
	// visibility_updated().

	if (data(IN_THREAD).actual_visibility ==
	    data(IN_THREAD).requested_visibility)
		return;

	visibility_updated(IN_THREAD,
			   (
			    data(IN_THREAD).actual_visibility=
			    data(IN_THREAD).requested_visibility));
}

void elementObj::implObj::visibility_updated(IN_THREAD_ONLY, bool flag)
{
	// This is called when this element's actual visibility changes.
	//
	// Note that child_element overrides it, and checks if the parent
	// container's inherited_visibility is false, and then overrides 'flag'
	// to false in that case.
	//
	// If the visibility matches the current inherited_visibility, do
	// nothing.

	if (data(IN_THREAD).inherited_visibility == flag)
		return;

	// Prepare the default inherited_visibility_info object: here's the
	// new visibility status, in "flag", and do_not_redraw is false,
	// because we certainly want to redraw the element, as a result of the
	// visibility change.

	inherited_visibility_info visibility_info{flag, false};

	inherited_visibility_updated(IN_THREAD, visibility_info);

	if (!visibility_info.do_not_redraw)
		draw_after_visibility_updated(IN_THREAD, flag);
}

void elementObj::implObj
::inherited_visibility_updated(IN_THREAD_ONLY,
			       inherited_visibility_info &visibility_info)
{
	// This is called when the element's inherited_visibility, the
	// "real" visibility, after taking into consideration the parent
	// display element's visibility, changes.
	//
	// This calls do_inherited_visibility_updated(). container_impl
	// overrides this, and also takes care of whatever needs to be done
	// with the child elements, in addition to calling
	// do_inherited_visibility_updated(), too.
	do_inherited_visibility_updated(IN_THREAD, visibility_info);
}

void elementObj::implObj
::do_inherited_visibility_updated(IN_THREAD_ONLY,
				  inherited_visibility_info &info)
{
	// Officially record the fact that this element is now visible, or
	// not visible, for real.
	//
	// Notify handlers that we're about to show or hide this element.

	invoke_element_state_updates(IN_THREAD,
				     info.flag
				     ? element_state::before_showing
				     : element_state::before_hiding);

	set_inherited_visibility(IN_THREAD, info);

	// Notify handlers that we just shown or hidden this element.

	invoke_element_state_updates(IN_THREAD,
				     info.flag
				     ? element_state::after_showing
				     : element_state::after_hiding);
}

void elementObj::implObj
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &info)
{
	set_inherited_visibility_flag(IN_THREAD, info.flag);
}

void elementObj::implObj
::set_inherited_visibility_flag(IN_THREAD_ONLY, bool flag)
{
	// Offically update this element's "real" visibility.
	data(IN_THREAD).inherited_visibility=flag;
	if (!flag)
	{
		unschedule_hover_action(IN_THREAD);

		// Also hide the popup.
		if (data(IN_THREAD).attached_popup)
			data(IN_THREAD).attached_popup
				->request_visibility(IN_THREAD, false);
	}
}

void elementObj::implObj::draw_after_visibility_updated(IN_THREAD_ONLY,
							bool flag)
{
	// Display the contents of this element.
	//
	// generic_window_handler overrides this, and maps or unmaps the
	// window. This is what this action means for actual windows.
	//
	// Otherwise we call schedule_redraw().
	schedule_redraw(IN_THREAD);
}

void elementObj::implObj::schedule_redraw(IN_THREAD_ONLY)
{
	if (!get_window_handler().has_exposed(IN_THREAD))
		return;

	IN_THREAD->elements_to_redraw(IN_THREAD)->insert(elementimpl(this));
}

void elementObj::implObj::schedule_redraw_recursively()
{
	THREAD->run_as([me=ref(this)]
		       (IN_THREAD_ONLY)
		       {
			       me->schedule_redraw_recursively(IN_THREAD);
		       });
}

void elementObj::implObj::schedule_redraw_recursively(IN_THREAD_ONLY)
{
	schedule_redraw(IN_THREAD);

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->schedule_redraw_recursively(IN_THREAD);
		       });
}

rectangle_set draw_info::entire_area() const
{
	if (absolute_location.width == 0 || absolute_location.height == 0)
		return {};

	return {{0, 0, absolute_location.width, absolute_location.height}};
}

rectangle elementObj::implObj::get_absolute_location_on_screen(IN_THREAD_ONLY)
{
	auto r=get_absolute_location(IN_THREAD);

	get_window_handler().get_absolute_location_on_screen(IN_THREAD, r);

	return r;
}

bool elementObj::implObj::redraw_scheduled(IN_THREAD_ONLY)
{
	auto elements_to_redraw=IN_THREAD->elements_to_redraw(IN_THREAD);

	return elements_to_redraw->find(ref(this)) != elements_to_redraw->end();
}

void elementObj::implObj::explicit_redraw(IN_THREAD_ONLY, draw_info_cache &c)
{
	// Remove myself from the connection thread's list.

	IN_THREAD->elements_to_redraw(IN_THREAD)->erase(ref<implObj>(this));

	// Invoke draw() to refresh the contents of this disiplay element.

	auto &di=get_draw_info(IN_THREAD);

	// Simulate an exposure of the entire element.

	draw(IN_THREAD, di, di.entire_area());
}

void elementObj::implObj
::on_state_update(const std::function<element_state_callback_t> &cb)
{
	THREAD->get_batch_queue()->run_as([cb, me=ref(this)]
		       (IN_THREAD_ONLY)
		       {
			       me->data(IN_THREAD).element_state_callback=cb;

			       cb(me->create_element_state
				  (IN_THREAD,
				   element_state::current_state),
				  busy_impl{*me});
		       });
}

void elementObj::implObj::set_minimum_override(dim_t horiz_override,
					       dim_t vert_override)
{
	THREAD->run_as([me=ref(this),
			horiz_override,
			vert_override]
		       (IN_THREAD_ONLY)
		       {
			       me->set_minimum_override(IN_THREAD,
							horiz_override,
							vert_override);
		       });
}

void elementObj::implObj::set_minimum_override(IN_THREAD_ONLY,
					       dim_t horiz_override,
					       dim_t vert_override)
{
	get_horizvert(IN_THREAD)
		->set_minimum_override(IN_THREAD,
				       horiz_override, vert_override);
}

void elementObj::implObj::update_current_position(IN_THREAD_ONLY,
						  const rectangle &r)
{
	auto &current_data=data(IN_THREAD);

	if (r == current_data.current_position)
		return;

	current_data.current_position=r;

	notify_updated_position(IN_THREAD);
	current_position_updated(IN_THREAD);
}

void elementObj::implObj::current_position_updated(IN_THREAD_ONLY)
{
	schedule_update_position_processing(IN_THREAD);

	// Well, if we changed position, so must be all child elements.

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->current_position_updated(IN_THREAD);
		       });
}

void elementObj::implObj::absolute_location_updated(IN_THREAD_ONLY)
{
	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->absolute_location_updated(IN_THREAD);
		       });
}

void elementObj::implObj::schedule_update_position_processing(IN_THREAD_ONLY)
{
	IN_THREAD->insert_element_set(*IN_THREAD->element_position_updated
				      (IN_THREAD),
				      elementimpl(this));
}

void elementObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	schedule_redraw(IN_THREAD);
}

void elementObj::implObj::notify_updated_position(IN_THREAD_ONLY)
{
	invoke_element_state_updates(IN_THREAD,
				     element_state::current_state);
}

element_state elementObj::implObj
::create_element_state(IN_THREAD_ONLY,
		       element_state::state_update_t element_state_for)
{
	auto &current_data=data(IN_THREAD);

	return element_state{
		element_state_for,
			// We send this update any time inherited visibility
			// changes, see do_inherited_visibility_updated().
		current_data.inherited_visibility,
			// We send this update any time current_position
			// changes. update_current_position() calls
			// notify_updated_position().
		current_data.current_position
			};
}

void elementObj::implObj
::invoke_element_state_updates(IN_THREAD_ONLY,
			       element_state::state_update_t reason)
{
	data(IN_THREAD).element_state_callback
		(create_element_state(IN_THREAD, reason),
		 busy_impl{*this});
}

clip_region_set::clip_region_set(IN_THREAD_ONLY,
				  const draw_info &di)
{
	di.window_picture->set_clip_rectangles(di.element_viewport);
}

void elementObj::implObj::prepare_draw_info(IN_THREAD_ONLY, draw_info &)
{
}

void elementObj::implObj::exposure_event_recursive(IN_THREAD_ONLY,
						   const rectangle_set &areas)
{
	auto &di=get_draw_info(IN_THREAD);

	// Any queued redraws are moot, now.
	IN_THREAD->elements_to_redraw(IN_THREAD)->erase(elementimpl(this));

#ifdef DEBUG_EXPOSURE_CALCULATIONS

	std::cout << "Exposure: " << objname() << ": "
		  << data(IN_THREAD).current_position << std::endl;

	for (const auto &r:areas)
		std::cout << "        " << r << std::endl;

	std::cout << "    Viewport:" << std::endl;

	for (const auto &r:di.element_viewport)
		std::cout << "        " << r << std::endl;
#endif
	// The intersection of areas, and the calculated viewport, is what
	// we need to draw.
	//
	// But draw() expects all coordinates relative to the display
	// element, and they're absolute now. No problem.

	auto draw_area=intersect(di.element_viewport, areas,
				 -di.absolute_location.x,
				 -di.absolute_location.y);

#ifdef DEBUG_EXPOSURE_CALCULATIONS

	std::cout << "    Draw:" << std::endl;

	for (const auto &r:draw_area)
		std::cout << "        " << r << std::endl;
#endif

	draw(IN_THREAD, di, draw_area);

	// Now, we need to recursively propagate this event.

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       e->impl->exposure_event_recursive(IN_THREAD,
								 areas);
		       });
}

void elementObj::implObj::draw(IN_THREAD_ONLY,
			       const draw_info &di,
			       const rectangle_set &areas)
{
	if (areas.empty() || di.element_viewport.empty())
		return; // Don't bother.

	if (DO_NOT_DRAW)
		clear_to_color(IN_THREAD, di, areas);
	else
		do_draw(IN_THREAD, di, areas);
}

void elementObj::implObj::do_draw(IN_THREAD_ONLY,
				  const draw_info &di,
				  const rectangle_set &areas)
{
	clear_to_color(IN_THREAD, di, areas);
}

void elementObj::implObj
::do_draw_using_scratch_buffer(IN_THREAD_ONLY,
			       const function<scratch_buffer_draw_func_t> &cb,
			       const rectangle &rect,
			       const draw_info &di,
			       const draw_info &background_color_di,
			       const clip_region_set &clipped)
{
	do_draw_using_scratch_buffer(IN_THREAD, cb, rect,
				     di, background_color_di, clipped,
				     element_scratch_buffer);
}

void elementObj::implObj
::do_draw_using_scratch_buffer(IN_THREAD_ONLY,
			       const function<scratch_buffer_draw_func_t> &cb,
			       const rectangle &rect,
			       const draw_info &di,
			       const draw_info &background_color_di,
			       const clip_region_set &clipped,
			       const scratch_buffer &buffer)
{
	if (di.no_viewport())
		return;

	buffer->get
		(rect.width,
		 rect.height,
		 [&, this]
		 (const picture &area_picture,
		  const pixmap &area_pixmap,
		  const gc &area_gc)
		 {
			 rectangle area_entire_rect{0, 0,
					 rect.width, rect.height};

			 auto bgxy=background_color_di.background_xy_to(di);

			 area_picture->impl
				 ->composite(background_color_di
					     .window_background,
					     coord_t::truncate(bgxy.first + rect.x),
					     coord_t::truncate(bgxy.second + rect.y),
					     area_entire_rect);

			 cb(area_picture, area_pixmap, area_gc);

			 this->draw_to_window_picture(IN_THREAD,
						      clipped,
						      di,
						      area_picture,
						      rect);
		 });

}

void elementObj::implObj
::draw_to_window_picture(IN_THREAD_ONLY,
			 const clip_region_set &set,
			 const draw_info &di,
			 const picture &contents,
			 const rectangle &rect)
{
	rectangle cpy=rect;

	cpy.x = coord_t::truncate(cpy.x + di.absolute_location.x);
	cpy.y = coord_t::truncate(cpy.y + di.absolute_location.y);

	auto &wh=get_window_handler();

	if (draw_to_window_picture_as_disabled(IN_THREAD) ||
	    set.draw_as_disabled)
	{
		// Disabled element rendering -- dither in the main window's
		// background color.

		auto &di=wh.get_draw_info(IN_THREAD);

		auto xy=di.background_xy_to(cpy.x, cpy.y);

		contents->impl->composite(di.window_background,
					  wh.disabled_mask(IN_THREAD)
					  ->image->icon_picture->impl,
					  xy.first, xy.second,
					  cpy.x, cpy.y,
					  0, 0,
					  cpy.width, cpy.height,
					  render_pict_op::op_over);
	}

	// If there's a busy mcguffin outstanding, and composition is available,
	// draw a shade on top of us.

	if (wh.is_busy() && wh.drawable_pictformat->alpha_depth > 0)
	{
		contents->composite(wh.shaded_color(IN_THREAD)
				    ->get_current_color(IN_THREAD),
				    cpy.x,
				    cpy.y,
				    {0, 0, cpy.width, cpy.height},
				    render_pict_op::op_atop);
	}

	di.window_picture->composite(contents->impl,
				     0, 0,
				     cpy);
}
void elementObj::implObj::clear_to_color(IN_THREAD_ONLY,
					 const draw_info &di,
					 const rectangle_set &areas)
{
	clear_to_color(IN_THREAD,
		       clip_region_set(IN_THREAD, di),
		       di, di, areas);
}

void elementObj::implObj::clear_to_color(IN_THREAD_ONLY,
					 const clip_region_set &clip,
					 const draw_info &di,
					 const draw_info &background_color_di,
					 const rectangle_set &areas)
{
#ifdef CLEAR_TO_COLOR_LOG
	CLEAR_TO_COLOR_LOG();
#endif

	// Take the viewport, and mask out the areas we're clearing.
	// If we have a large element inside a peephole, this avoids us having
	// to allocate a huge scratch buffer, with most of it being unused.

	rectangle_set absareas;

	for (auto area:di.element_viewport)
	{
		area.x = coord_t::truncate(area.x-di.absolute_location.x);
		area.y = coord_t::truncate(area.y-di.absolute_location.y);
		absareas.insert(area);
	}

	absareas=intersect(absareas, areas);

	for (const auto &area : absareas)
	{
#ifdef CLEAR_TO_COLOR_RECT
		CLEAR_TO_COLOR_RECT();
#endif
		draw_using_scratch_buffer
			(IN_THREAD,
			 []
			 (const auto &, const auto &, const auto &)
			 {
			 },
			 area,
			 di, background_color_di,
			 clip);
	}
}

void elementObj::implObj::remove_background_color()
{
	THREAD->run_as([impl=ref<implObj>(this)]
		       (IN_THREAD_ONLY)
		       {
			       impl->remove_background_color(IN_THREAD);
		       });
}

void elementObj::implObj
::set_background_color(const std::string_view &name)
{
	set_background_color(get_screen()->impl
			     ->create_background_color(name));
}

void elementObj::implObj
::set_background_color(const const_picture &background_color)
{
	set_background_color(get_screen()->impl
			     ->create_background_color(background_color));
}

void elementObj::implObj
::set_background_color(const background_color &c)
{
	THREAD->run_as([impl=ref<implObj>(this), c]
		       (IN_THREAD_ONLY)
		       {
			       impl->set_background_color(IN_THREAD, c);
		       });
}

bool elementObj::implObj::has_own_background_color(IN_THREAD_ONLY)
{
	return true;
}

void elementObj::implObj::background_color_changed(IN_THREAD_ONLY)
{
	if (!data(IN_THREAD).inherited_visibility)
		return;

	schedule_redraw(IN_THREAD);

	for_each_child(IN_THREAD, [&]
		       (const element &e)
		       {
			       if (e->impl->data(IN_THREAD)
				   .inherited_visibility &&
				   e->impl->has_own_background_color(IN_THREAD))
				       return;
			       e->impl->background_color_changed(IN_THREAD);
		       });
}

void elementObj::implObj::theme_updated(IN_THREAD_ONLY,
					const defaulttheme &new_theme)
{
	if (data(IN_THREAD).inherited_visibility)
		schedule_redraw(IN_THREAD);

	for_each_child(IN_THREAD, [&]
		       (const element &e)
		       {
			       e->impl->theme_updated(IN_THREAD, new_theme);
		       });
}

void elementObj::implObj::initialize_or_log_exception(IN_THREAD_ONLY)
{
	try {
		initialize(IN_THREAD);
	} CATCH_EXCEPTIONS;
}

void elementObj::implObj::initialize(IN_THREAD_ONLY)
{
}

void elementObj::implObj::do_for_each_child(IN_THREAD_ONLY,
					    const function<void
					    (const element &e)> &)
{
}

const char *elementObj::implObj::label_theme_font() const
{
	return "label";
}

current_fontcollection elementObj::implObj::create_font(const font &props)
{
	return get_window_handler().create_font(props);
}

current_fontcollection elementObj::implObj
::create_theme_font(const std::string_view &font)
{
	return get_window_handler().create_theme_font(font);
}

background_color elementObj::implObj
::create_background_color(const std::string_view &color_name)
{
	return get_screen()->impl->create_background_color(color_name);
}

background_color elementObj::implObj::create_background_color(const rgb &color)
{
	return get_screen()->impl->create_background_color
		(get_screen()->create_solid_color_picture(color));
}

bool elementObj::implObj::enabled(IN_THREAD_ONLY)
{
	// This element has been removed from the container.
	//
	// The destructor of the public object will make sure that
	// the focus has been properly removed from me. But, when an
	// entire container is removed, remove() recursively sets the removed
	// flag on the container's entire contents, and this is going to
	// prevent the input focus from bouncing until it escapes the
	// elements that are being destroyed.

	if (data(IN_THREAD).removed || !data(IN_THREAD).inherited_visibility)
		return false;

	return data(IN_THREAD).enabled;
}

void elementObj::implObj::on_keyboard_focus(const
					    std::function<focus_callback_t>
					    &callback)
{
	THREAD->run_as([me=ref(this), callback]
		       (IN_THREAD_ONLY)
		       {
			       me->on_keyboard_focus(IN_THREAD, callback);
		       });
}

void elementObj::implObj::on_keyboard_focus(IN_THREAD_ONLY,
					    const
					    std::function<focus_callback_t>
					    &callback)
{
	data(IN_THREAD).on_keyboard_callback=callback;
	invoke_keyboard_focus_callback(IN_THREAD);
}

void elementObj::implObj::report_keyboard_focus(IN_THREAD_ONLY,
					       focus_change event)
{
	if (event != focus_change::focus_movement_complete)
	{
		most_recent_keyboard_focus_change(IN_THREAD)=event;
		return;
	}

	keyboard_focus(IN_THREAD);
}


void elementObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	unschedule_hover_action(IN_THREAD);
	invoke_keyboard_focus_callback(IN_THREAD);
}

void elementObj::implObj::invoke_keyboard_focus_callback(IN_THREAD_ONLY)
{
	try {
		data(IN_THREAD).on_keyboard_callback
			(most_recent_keyboard_focus_change(IN_THREAD));
	} CATCH_EXCEPTIONS;
}

void elementObj::implObj::on_pointer_focus(const
					    std::function<focus_callback_t>
					    &callback)
{
	THREAD->run_as([me=ref(this), callback]
		       (IN_THREAD_ONLY)
		       {
			       me->on_pointer_focus(IN_THREAD, callback);
		       });
}

void elementObj::implObj::on_pointer_focus(IN_THREAD_ONLY,
					    const
					    std::function<focus_callback_t>
					    &callback)
{
	data(IN_THREAD).on_pointer_callback=callback;
	invoke_pointer_focus_callback(IN_THREAD);
}

void elementObj::implObj::report_pointer_focus(IN_THREAD_ONLY,
					       focus_change event)
{
	if (event != focus_change::focus_movement_complete)
	{
		most_recent_pointer_focus_change(IN_THREAD)=event;
		return;
	}

	pointer_focus(IN_THREAD);
}

void elementObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	unschedule_hover_action(IN_THREAD);
	invoke_pointer_focus_callback(IN_THREAD);
}

void elementObj::implObj::invoke_pointer_focus_callback(IN_THREAD_ONLY)
{
	try {
		data(IN_THREAD).on_pointer_callback
			(most_recent_pointer_focus_change(IN_THREAD));
	} CATCH_EXCEPTIONS;
}

void elementObj::implObj::window_focus_change(IN_THREAD_ONLY, bool flag)
{
}

bool elementObj::implObj::current_keyboard_focus(IN_THREAD_ONLY)
{
	return in_focus(most_recent_keyboard_focus_change(IN_THREAD));
}

bool elementObj::implObj::current_pointer_focus(IN_THREAD_ONLY)
{
	return in_focus(most_recent_pointer_focus_change(IN_THREAD));
}

bool in_focus(focus_change v)
{
	return v != focus_change::lost && v != focus_change::child_lost;
}

void elementObj::implObj
::on_key_event(const std::function<key_event_callback_t> &cb)
{
	THREAD->run_as([me=ref(this), cb]
		       (IN_THREAD_ONLY)
		       {
			       me->on_key_event(IN_THREAD, cb);
		       });
}

void elementObj::implObj
::on_key_event(IN_THREAD_ONLY,
	       const std::function<key_event_callback_t> &cb)
{
	data(IN_THREAD).on_key_event_callback=cb;
}

bool elementObj::implObj::activate_for(const key_event &ke) const
{
	return ke.keypress;
}

bool elementObj::implObj::activate_for(const button_event &be) const
{
	return be.press;
}

bool elementObj::implObj::process_key_event(IN_THREAD_ONLY, const key_event &e)
{
	busy_impl mcguffin{*this};

	return data(IN_THREAD).on_key_event_callback(&e, mcguffin);
}

bool elementObj::implObj::uses_input_method()
{
	return false;
}

void elementObj::implObj::report_current_cursor_position(IN_THREAD_ONLY,
							 rectangle pos)
{
	auto loc=get_absolute_location(IN_THREAD);

	pos.x=coord_t::truncate(pos.x+loc.x);
	pos.y=coord_t::truncate(pos.y+loc.y);

	get_window_handler().with_xim_client
		([&]
		 (auto &client)
		 {
			 client->current_cursor_position(IN_THREAD, pos);
		 });
}

void elementObj::implObj::report_motion_event(IN_THREAD_ONLY,
					      const motion_event &me)
{
	data(IN_THREAD).last_motion_x=me.x;
	data(IN_THREAD).last_motion_y=me.y;

	// If a tooltip is installed, uninstall it before scheduling the
	// tooltip again.
	if (me.type == motion_event_type::real_motion)
	{
		unschedule_hover_action(IN_THREAD);

		if (me.mask.ordinal() == 0)
			schedule_hover_action(IN_THREAD);
	}
}

void elementObj::implObj::schedule_hover_action(IN_THREAD_ONLY)
{
	auto &d=data(IN_THREAD);

	auto initial_delay=hover_action_delay(IN_THREAD);

	if (initial_delay == std::chrono::milliseconds{0})
		return;

	auto now=tick_clock_t::now();

	d.hover_scheduled_creation=now+initial_delay;

	if (d.hover_scheduled_mcguffin)
		return; // Timer already set.

	schedule_hover_timer(IN_THREAD, now);
}

void elementObj::implObj::schedule_hover_timer(IN_THREAD_ONLY,
						 tick_clock_t::time_point now)
{
	data(IN_THREAD).hover_scheduled_mcguffin=IN_THREAD->schedule_callback
		(IN_THREAD,
		 now < data(IN_THREAD).hover_scheduled_creation
		 ? data(IN_THREAD).hover_scheduled_creation - now
		 : ++tick_clock_t::duration::zero(),
		 [me=make_weak_capture(ref<implObj>(this))]
		 (IN_THREAD_ONLY)
		 {
			 me.get([&]
				(const auto &me) {
					me->check_hover_timer(IN_THREAD);
				});
		 });
}

void elementObj::implObj::check_hover_timer(IN_THREAD_ONLY)
{
	// The timer to show the tooltip gets scheduled on a motion event.
	// For optimal performance the timer does not get rescheduled with
	// every event. Rather we save the hover_scheduled_creation time,
	// and now that the timer expired we'll check if the
	// hover_scheduled_creation time was reset, then try again.

	auto now=tick_clock_t::now();

	auto &d=data(IN_THREAD);
	d.hover_scheduled_mcguffin=nullptr; // Clear the expired mcguffin.

	// Recheck things.

	if (hover_action_delay(IN_THREAD) == std::chrono::milliseconds{0})
		return;

	if (now < d.hover_scheduled_creation)
	{
		schedule_hover_timer(IN_THREAD, now);
		return; // Reschedule me.
	}

	hover_action(IN_THREAD);
}

void elementObj::implObj::unschedule_hover_action(IN_THREAD_ONLY)
{
	data(IN_THREAD).hover_scheduled_mcguffin=nullptr;

	hover_cancel(IN_THREAD);
}

void elementObj::implObj::ensure_visibility(IN_THREAD_ONLY, const rectangle &r)
{
}

void elementObj::implObj::ensure_entire_visibility(IN_THREAD_ONLY)
{
	ensure_visibility(IN_THREAD, {0, 0,
				data(IN_THREAD).current_position.width,
				data(IN_THREAD).current_position.height});
}


bool elementObj::implObj::pasted(IN_THREAD_ONLY,
				 const std::u32string_view &str)
{
	busy_impl mcguffin{*this};
	return data(IN_THREAD).on_key_event_callback(&str, mcguffin);
}

void elementObj::implObj::creating_focusable_element()
{
}

//////////////////////////////////////////////////////////////////////////////

void elementObj::implObj::set_cursor_pointer(IN_THREAD_ONLY,
					     const cursor_pointer &p)
{
	if (data(IN_THREAD).pointer != p)
	{
		data(IN_THREAD).pointer=p;
		get_window_handler().update_displayed_cursor_pointer(IN_THREAD);
	}
}

void elementObj::implObj::remove_cursor_pointer(IN_THREAD_ONLY)
{
	if (data(IN_THREAD).pointer)
	{
		data(IN_THREAD).pointer=nullptr;
		get_window_handler().update_displayed_cursor_pointer(IN_THREAD);
	}
}

cursor_pointerptr elementObj::implObj::get_cursor_pointer(IN_THREAD_ONLY)
{
	return data(IN_THREAD).pointer;
}

LIBCXXW_NAMESPACE_END
