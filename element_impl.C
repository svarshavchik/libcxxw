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
#include "background_color.H"
#include "x/w/element_state.H"
#include "x/w/scratch_buffer.H"
#include "x/callback_list.H"
#include "element_screen.H"
#include "fonts/current_fontcollection.H"
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "x/w/text_param.H"
#include <x/logger.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::elementObj::implObj);

LIBCXXW_NAMESPACE_START

#define THREAD get_window_handler().screenref->impl->thread

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

void elementObj::implObj::request_visibility(bool flag)
{
	// Set requested_visibility, make sure this is done in the connection
	// thread.

	// Sets requested_visibility, then adds the element to the
	// visibility_updated list.

	// The connection thread invokes update_visibility after processing
	// all messages.

	THREAD->get_batch_queue()->run_as
		(RUN_AS,
		 [flag, me=elementimpl(this)]
		 (IN_THREAD_ONLY)
		 {
			 me->request_visibility(IN_THREAD, flag);
		 });
}

void elementObj::implObj::request_visibility(IN_THREAD_ONLY, bool flag)
{
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
		(RUN_AS,
		 [flag, me=elementimpl(this)]
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
				     data(IN_THREAD).requested_visibility
				     ? element_state::after_showing
				     : element_state::after_hiding);
}

void elementObj::implObj
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &info)
{
	// Offically update this element's "real" visibility.
	data(IN_THREAD).inherited_visibility=info.flag;
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

void elementObj::implObj::schedule_redraw_if_visible(IN_THREAD_ONLY)
{
	if (!data(IN_THREAD).inherited_visibility)
		return;
	schedule_redraw(IN_THREAD);
}

void elementObj::implObj::schedule_redraw(IN_THREAD_ONLY)
{
	IN_THREAD->elements_to_redraw(IN_THREAD)->insert(elementimpl(this));
}

void elementObj::implObj::explicit_redraw(IN_THREAD_ONLY, draw_info_cache &c)
{
	// Remove myself from the connection thread's list.

	IN_THREAD->elements_to_redraw(IN_THREAD)->erase(ref<implObj>(this));

	// Invoke draw() to refresh the contents of this disiplay element.

	auto &di=get_draw_info(IN_THREAD);

	// Simulate an exposure of the entire element.

	rectangle_set entire_area;

	entire_area.insert({0, 0, data(IN_THREAD).current_position.width,
				data(IN_THREAD).current_position.height});

	if (data(IN_THREAD).inherited_visibility)
	{
		draw(IN_THREAD, di, entire_area);
	}
	else
	{
		clear_to_color(IN_THREAD, di, entire_area);
	}
}

ref<obj> elementObj::implObj
::do_on_state_update(const element_state_update_handler_t &handler)
{
	// It's ok, create_mcguffin() can be safely used by any thread.
	auto mcguffin=data_thread_only.update_handlers->create_mcguffin();

	THREAD->run_as(RUN_AS,
		       [mcguffin, handler,
			me=ref<elementObj::implObj>(this)]
		       (IN_THREAD_ONLY)
		       {
			       // Install the new handler in the connection
			       // thread, then send it the current_state
			       mcguffin->install(handler);

			       handler->invoke(me->create_element_state
					       (IN_THREAD,
						element_state::current_state));
		       });

	return mcguffin;
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

void elementObj::implObj::schedule_update_position_processing(IN_THREAD_ONLY)
{
	IN_THREAD->insert_element_set(*IN_THREAD->element_position_updated
				      (IN_THREAD),
				      elementimpl(this));
}

void elementObj::implObj::process_updated_position(IN_THREAD_ONLY)
{
	schedule_redraw_if_visible(IN_THREAD);
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
		current_data.actual_visibility,
		current_data.current_position
			};
}

void elementObj::implObj
::invoke_element_state_updates(IN_THREAD_ONLY,
			       element_state::state_update_t reason)
{
	data(IN_THREAD).update_handlers->invoke(create_element_state
						(IN_THREAD, reason));
}

clip_region_set::clip_region_set(IN_THREAD_ONLY,
				  const draw_info &di)
{
	if (di.element_viewport.width != 0 &&
	    di.element_viewport.height != 0)
	{
		// This now clips the subsequent draw operation to this
		// display element's viewport.
		di.window_picture
			->set_clip_rectangle(di.element_viewport);
	}
	else
	{
		// Clip everything.

		di.window_picture->set_clip_rectangles(rectangle_set());
	}
}

void elementObj::implObj::prepare_draw_info(IN_THREAD_ONLY, draw_info &)
{
}

void elementObj::implObj::exposure_event_recursive(IN_THREAD_ONLY,
						   rectangle_set &areas)
{
	auto &di=get_draw_info(IN_THREAD);

	draw(IN_THREAD, di, areas);

	// Now, we need to recursively propagate this event.

	for_each_child(IN_THREAD,
		       [&]
		       (const element &e)
		       {
			       if (!e->impl->data(IN_THREAD).actual_visibility)
				       return;

			       auto child_position=e->impl->data(IN_THREAD)
				       .current_position;

			       auto child_areas=
				       intersect(areas, {child_position},
						 -child_position.x,
						 -child_position.y);

			       if (child_areas.empty())
				       return;

			       e->impl->exposure_event_recursive(IN_THREAD,
								 child_areas);
		       });
}

void elementObj::implObj::draw(IN_THREAD_ONLY,
			       const draw_info &di,
			       const rectangle_set &areas)
{
	if (data(IN_THREAD).inherited_visibility)
	{
		do_draw(IN_THREAD, di, areas);
		return;
	}
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
			 const const_picture &contents,
			 const rectangle &rect)
{
	rectangle cpy=rect;

	cpy.x = coord_t::truncate(cpy.x + di.absolute_location.x);
	cpy.y = coord_t::truncate(cpy.y + di.absolute_location.y);

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

	for (const auto &area:areas)
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
	get_screen()->impl->thread->get_batch_queue()
		->run_as(RUN_AS,
			 [impl=ref<implObj>(this)]
			 (IN_THREAD_ONLY)
			 {
				 impl->remove_background_color(IN_THREAD);
			 });
}

void elementObj::implObj
::set_background_color(const std::experimental::string_view &name,
		       const rgb &default_value)
{
	set_background_color(get_screen()->impl
			     ->create_background_color(name, default_value));
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
	get_screen()->impl->thread->get_batch_queue()
		->run_as(RUN_AS,
			 [impl=ref<implObj>(this), c]
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
	schedule_redraw_if_visible(IN_THREAD);

	for_each_child(IN_THREAD, [&]
		       (const element &e)
		       {
			       if (!e->impl->data(IN_THREAD)
				   .inherited_visibility)
				       return;

			       if (e->impl->has_own_background_color(IN_THREAD))
				       return;
			       e->impl->background_color_changed(IN_THREAD);
		       });
}

void elementObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	schedule_redraw_if_visible(IN_THREAD);

	for_each_child(IN_THREAD, [&]
		       (const element &e)
		       {
			       if (!e->impl->data(IN_THREAD)
				   .inherited_visibility)
				       return;

			       e->impl->theme_updated(IN_THREAD);
		       });
}

void elementObj::implObj::initialize(IN_THREAD_ONLY)
{
}

void elementObj::implObj::do_for_each_child(IN_THREAD_ONLY,
					    const function<void
					    (const element &e)> &)
{
}

current_fontcollection elementObj::implObj::create_font(const font &props)
{
	return get_window_handler().create_font(props);
}

current_fontcollection elementObj::implObj
::create_theme_font(const std::experimental::string_view &font)
{
	return get_window_handler().create_theme_font(font);
}

richtextstring elementObj::implObj::convert(richtextmeta font,
					    const text_param &t)
{
	// Compute all offsets into the richtextstring where fonts or colors
	// change.

	std::set<size_t> all_positions;

	for (const auto &p:t.fonts)
		all_positions.insert(p.first);
	for (const auto &p:t.theme_fonts)
		all_positions.insert(p.first);
	for (const auto &p:t.colors)
		all_positions.insert(p.first);
	for (const auto &p:t.background_colors)
		all_positions.insert(p.first);
	for (const auto &p:t.theme_colors)
		all_positions.insert(p.first);
	for (const auto &p:t.theme_background_colors)
		all_positions.insert(p.first);

	// Now iterate over them, in order, to create the unordered_map for
	// richtextstring's constructor.
	std::unordered_map<size_t, richtextmeta> m;

	bool inserted_default=false;

	auto screen_impl=get_screen()->impl;

	for (const auto &p:all_positions)
	{
		if (p > 0 && !inserted_default)
			m.insert({0, font});
		inserted_default=true;

		{
			auto iter=t.fonts.find(p);

			if (iter != t.fonts.end())
				font=font.replace_font(create_font
						       (iter->second));
		}

		{
			auto iter=t.theme_fonts.find(p);

			if (iter != t.theme_fonts.end())
				font=font.replace_font(create_theme_font
						       (iter->second));
		}

		{
			auto iter=t.colors.find(p);

			if (iter != t.colors.end())
			{
				font.textcolor=screen_impl
					->create_background_color
					(screen_impl->create_solid_color_picture
					 (iter->second));
				font.bg_color=background_colorptr();
			}
		}

		{
			auto iter=t.theme_colors.find(p);

			if (iter != t.theme_colors.end())
			{
				font.textcolor=screen_impl
					->create_background_color
					(iter->second, rgb());
				font.bg_color=background_colorptr();
			}
		}


		{
			auto iter=t.background_colors.find(p);

			if (iter != t.background_colors.end())
			{
				font.bg_color=screen_impl
					->create_background_color
					(screen_impl->create_solid_color_picture
					 (iter->second));
			}
		}

		{
			auto iter=t.theme_background_colors.find(p);

			if (iter != t.theme_background_colors.end())
				font.bg_color=screen_impl
					->create_background_color
					(iter->second, rgb());
		}

		m.insert({p, font});
	}

	return richtextstring{t.string, m};
}

LIBCXXW_NAMESPACE_END
