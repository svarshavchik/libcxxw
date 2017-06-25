/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "connectionfwd.H"
#include "pictformat.H"
#include "icon.H"
#include "draw_info.H"
#include "draw_info_cache.H"
#include "container_element.H"
#include "layoutmanager.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "xid_t.H"
#include "element_screen.H"
#include "background_color.H"
#include "focus/focusable.H"
#include "grabbed_pointer.H"
#include "xim/ximclient.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/values_and_mask.H"
#include <X11/keysym.h>
#include "child_element.H"
#include "hotspot.H"
#include "catch_exceptions.H"
#include <x/property_value.H>
#include <x/strtok.H>
#include <x/chrcasecmp.H>

LIBCXXW_NAMESPACE_START

static property::value<bool> disable_grabs(LIBCXX_NAMESPACE_STR
					   "::w::disable_grab", false);

static rectangle element_position(const rectangle &r)
{
	auto cpy=r;

	cpy.x=0;
	cpy.y=0;
	return cpy;
}

static background_color default_background_color(const screen &s)
{
	return s->impl
		->create_background_color("mainwindow_background",
					  DEFAULT_MAINWINDOW_BACKGROUND_COLOR);
}

//////////////////////////////////////////////////////////////////////////


generic_windowObj::handlerObj::frame_extents_t
::frame_extents_t(const rectangle &workarea)
	: workarea(workarea)
{
}

bool generic_windowObj::handlerObj::frame_extents_t
::operator==(const frame_extents_t &o)
{
	return workarea == o.workarea &&
		left == o.left && right == o.right &&
		top == o.top && bottom == o.bottom;
}

//////////////////////////////////////////////////////////////////////////

static inline generic_windowObj::handlerObj::constructor_params
create_constructor_params(const screen &parent_screen)
{
	rectangle dimensions={0, 0, 1, 1};

	values_and_mask vm(XCB_CW_EVENT_MASK,
			   (uint32_t)
			   generic_windowObj::handlerObj::initial_event_mask(),
			   XCB_CW_COLORMAP,
			   parent_screen->impl->toplevelwindow_colormap->id(),
			   XCB_CW_BORDER_PIXEL,
			   parent_screen->impl->xcb_screen->black_pixel);

	return {
		{
			parent_screen,
			parent_screen->impl->xcb_screen->root, // parent
			parent_screen->impl->toplevelwindow_pictformat->depth, // depth
			dimensions, // initial_position
			XCB_WINDOW_CLASS_INPUT_OUTPUT, // window_class
			parent_screen->impl->toplevelwindow_visual->impl->visual_id, // visual
			vm, // events_and_mask
		},
		parent_screen->impl->toplevelwindow_pictformat
	};
}

generic_windowObj::handlerObj::handlerObj(IN_THREAD_ONLY,
					  const screen &parent_screen)
	: handlerObj(IN_THREAD, create_constructor_params(parent_screen))
{
}

generic_windowObj::handlerObj
::handlerObj(IN_THREAD_ONLY,
	     const constructor_params &params)
	: // This sets up the xcb_window_t
	  window_handlerObj(IN_THREAD,
			    params.window_handler_params),
	  // And we inherit it as the xcb_drawable_t
	  drawableObj::implObj(IN_THREAD,
			       window_handlerObj::id(),
			       params.drawable_pictformat),

	// We can now construct a picture for the window
	pictureObj::implObj::fromDrawableObj(IN_THREAD,
					     window_handlerObj::id(),
					     params.drawable_pictformat->impl
					     ->id),

	container_elementObj<elementObj::implObj>
	(0,
	 element_position(params.window_handler_params.initial_position),
	 params.window_handler_params.screenref,
	 params.drawable_pictformat,
	 "background@libcxx"),
	current_events_thread_only((xcb_event_mask_t)
				   params.window_handler_params
				   .events_and_mask.m.at(XCB_CW_EVENT_MASK)),
	current_position(params.window_handler_params.initial_position),
	disabled_mask_thread_only(create_icon_mm("disabled_mask",
						 render_repeat::normal,
						 0, 0)),
	current_background_color_thread_only(default_background_color
					     (params.window_handler_params
					      .screenref)),
	frame_extents_thread_only(params.window_handler_params.screenref
				  ->get_workarea())
{
}

generic_windowObj::handlerObj::~handlerObj()=default;

void generic_windowObj::handlerObj::installed(IN_THREAD_ONLY)
{
	mpobj<ewmh>::lock lock(screenref->get_connection()
			       ->impl->ewmh_info);

	lock->request_frame_extents(screenref->impl->screen_number, id());
}

////////////////////////////////////////////////////////////////////
//
// Inherited from elementObj::implObj

generic_windowObj::handlerObj &
generic_windowObj::handlerObj::get_window_handler()
{
	return *this;
}

const generic_windowObj::handlerObj &
generic_windowObj::handlerObj::get_window_handler() const
{
	return *this;
}

draw_info &generic_windowObj::handlerObj::get_draw_info(IN_THREAD_ONLY)
{
	auto &c=*IN_THREAD->current_draw_info_cache(IN_THREAD);
	auto e=ref<elementObj::implObj>(this);

	auto iter=c.draw_info_cache.find(e);

	if (iter != c.draw_info_cache.end())
		return iter->second;

	auto &viewport=data(IN_THREAD).current_position;

	return c.draw_info_cache.insert({e, {
			picture_internal(this),
			viewport,
			{viewport}, // No parent, everything is visible.
			current_background_color(IN_THREAD)
				->get_current_color(IN_THREAD)
				->impl,
			0,
			0,
	       }}).first->second;
}

rectangle generic_windowObj::handlerObj
::get_absolute_location(IN_THREAD_ONLY)
{
	return data(IN_THREAD).current_position;
}

void generic_windowObj::handlerObj
::draw_child_elements_after_visibility_updated(IN_THREAD_ONLY, bool flag)
{
}

void generic_windowObj::handlerObj
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &visibility_info)
{
	// We establish passive grabs for any button or keypress.

	// button_press_event() and key_press_event() will take care of
	// releasing the grabs.

	if (visibility_info.flag)
	{
#ifdef MAP_LOG
		MAP_LOG();
#endif
		// First convenient time to initialize the icon, as per
		// the contract.
		disabled_mask(IN_THREAD)=
			disabled_mask(IN_THREAD)->initialize(IN_THREAD);
		if (!disable_grabs.getValue())
		{
			xcb_grab_button(IN_THREAD->info->conn,
					false,
					id(),
					current_events(IN_THREAD) &
					(XCB_EVENT_MASK_BUTTON_PRESS |
					 XCB_EVENT_MASK_BUTTON_RELEASE |
					 XCB_EVENT_MASK_ENTER_WINDOW |
					 XCB_EVENT_MASK_LEAVE_WINDOW |
					 XCB_EVENT_MASK_POINTER_MOTION),
					XCB_GRAB_MODE_SYNC,
					XCB_GRAB_MODE_SYNC,
					XCB_NONE,
					XCB_NONE,
					0,
					XCB_MOD_MASK_ANY);

			xcb_grab_key(IN_THREAD->info->conn,
				     false,
				     id(),
				     XCB_MOD_MASK_ANY,
				     0,
				     XCB_GRAB_MODE_SYNC,
				     XCB_GRAB_MODE_SYNC);
		}
		xcb_map_window(IN_THREAD->info->conn, id());
		visibility_info.do_not_redraw=true;
	}
	else
	{
		xcb_unmap_window(IN_THREAD->info->conn, id());
		grab_locked(IN_THREAD)=false;
		release_grabs(IN_THREAD);
		xcb_ungrab_key(IN_THREAD->info->conn,
			       0,
			       id(),
			       XCB_MOD_MASK_ANY);

		xcb_ungrab_button(IN_THREAD->info->conn,
				  0,
				  id(),
				  XCB_MOD_MASK_ANY);
	}

	elementObj::implObj::set_inherited_visibility(IN_THREAD,
						      visibility_info);
}

void generic_windowObj::handlerObj::remove_background_color(IN_THREAD_ONLY)
{
	current_background_color(IN_THREAD)=
		default_background_color(get_screen());
	background_color_changed(IN_THREAD);
}

void generic_windowObj::handlerObj::set_background_color(IN_THREAD_ONLY,
							 const background_color
							 &c)
{
	current_background_color(IN_THREAD)=c;
	background_color_changed(IN_THREAD);
}

///////////////////////////////////////////////////////////////////////////////
//
// Inherited from window_handler

void generic_windowObj::handlerObj::exposure_event(IN_THREAD_ONLY,
						   const rectangle_set &areas)
{
	has_exposed(IN_THREAD)=true;
	exposure_event_recursive(IN_THREAD, areas);
}

void generic_windowObj::handlerObj::theme_updated_event(IN_THREAD_ONLY)
{
	// container_element_overrides_decl hijacks theme_updated(), so we
	// just do this here.

	theme_updated(IN_THREAD);
	current_background_color(IN_THREAD)->theme_updated(IN_THREAD);
}

bool generic_windowObj::handlerObj::is_busy()
{
	return !!busy_mcguffin_t::lock{busy_mcguffin}->getptr();
}

void generic_windowObj::handlerObj
::key_press_event(IN_THREAD_ONLY,
		  const xcb_key_press_event_t *event,
		  uint16_t sequencehi)
{
	grab_locked(IN_THREAD)=false;
	release_grabs(IN_THREAD); // Any previous grabs.

	// We grab_button()ed and grab_key()ed.
	// Make sure we'll release the grab, when the dust settles.

	grabbed_timestamp(IN_THREAD)=event->time;

	if (is_busy())
		// We're busy now. Since we're grabbing all key presses this
		// can only be checked now, after the grab processing.
		return;

	forward_key_event(IN_THREAD, event, sequencehi, true);
}

void generic_windowObj::handlerObj
::key_release_event(IN_THREAD_ONLY,
		    const xcb_key_release_event_t *event,
		    uint16_t sequencehi)
{
	grab_locked(IN_THREAD)=false;
	release_grabs(IN_THREAD); // Any previous grabs.
	forward_key_event(IN_THREAD, event, sequencehi, false);
}

void generic_windowObj::handlerObj
::forward_key_event(IN_THREAD_ONLY,
		    const xcb_key_release_event_t *event,
		    uint16_t sequencehi,
		    bool keypress)
{
	bool forwarded=false;

	if (most_recent_keyboard_focus(IN_THREAD) &&
	    most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
	    .uses_input_method())
	{
		with_xim_client
			([&]
			 (const auto &client)
			 {
				 forwarded=keypress ?
					 client->forward_key_press_event
					 (IN_THREAD, *event, sequencehi)
					 :
					 client->forward_key_release_event
					 (IN_THREAD, *event, sequencehi);
			 });
	}
	if (forwarded)
		return;

	handle_key_event(IN_THREAD, event, keypress);
}

void generic_windowObj::handlerObj
::handle_key_event(IN_THREAD_ONLY,
		   const xcb_key_release_event_t *event,
		   bool keypress)
{
	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	key_event ke{event->state, keysyms};

	ke.keypress=keypress;

	bool has_unicode=keysyms.lookup(event->detail, ke,
				      ke.unicode, ke.keysym);

	if (!has_unicode)
		ke.unicode=0;

	// If there's an element with a focus, delegate this to it. If
	// it doesn't process the key event, it'll eventually percolate back
	// to us.
	//
	// If there's no element in the focus, jump to our handler,
	// directly.

	bool processed;

	if (most_recent_keyboard_focus(IN_THREAD))
		processed=most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
			.process_key_event(IN_THREAD, ke);
	else
		processed=process_key_event(IN_THREAD, ke);

	// Check for shortcuts, as the last resort
	if (processed)
		return;

	auto shortcuts=shortcut_lookup(IN_THREAD).equal_range(ke.unicode);

	while (shortcuts.first != shortcuts.second)
	{
		auto best_shortcut=shortcuts.first->second;

		++shortcuts.first;

		if (!best_shortcut->get_hotspot_focusable()
		    .enabled(IN_THREAD)
		    ||
		    !best_shortcut->get_shortcut(IN_THREAD).matches(ke))
			continue;

		// If there are shortcuts for both shift-Foo and
		// shift-ctrl-Foo, make sure that shift-ctrl-Foo matches
		// the right shortcut.

		auto best_ordinal=
			best_shortcut->get_shortcut(IN_THREAD)
			.ordinal();

		while (shortcuts.first != shortcuts.second)
		{
			auto p=shortcuts.first->second;

			++shortcuts.first;

			if (!p->get_hotspot_focusable()
			    .enabled(IN_THREAD)
			    ||
			    !p->get_shortcut(IN_THREAD).matches(ke))
				continue;

			auto ordinal=p->get_shortcut(IN_THREAD)
				.ordinal();

			if (ordinal < best_ordinal)
				continue;

			best_shortcut=p;
			best_ordinal=ordinal;
		}

		try {
			best_shortcut->activated(IN_THREAD);
		} CATCH_EXCEPTIONS;
	}
}

void generic_windowObj::handlerObj
::button_press_event(IN_THREAD_ONLY,
		     const xcb_button_press_event_t *event)
{
	grab_locked(IN_THREAD)=false;
	release_grabs(IN_THREAD); // Any previous grabs.

	// We grab_button()ed and grab_key()ed.
	// Make sure we'll release the grab, when the dust settles.

	grabbed_timestamp(IN_THREAD)=event->time;

	if (is_busy())
		// We're busy now. Since we're grabbing all key presses this
		// can only be checked now, after the grab processing.
		return;
	do_button_event(IN_THREAD, event, true);
}

void generic_windowObj::handlerObj
::button_release_event(IN_THREAD_ONLY,
		       const xcb_button_release_event_t *event)
{
	// We need to remove all the grab first, in the case that the
	// code that processed the button release event wants to regrab
	// the pointer, for some reason. However we need to use the
	// current grab status for do_button_event(), so save it first.

	bool was_grabbed=grab_locked(IN_THREAD);
	grab_locked(IN_THREAD)=false;
	release_grabs(IN_THREAD); // Any previous grabs.

	do_button_event(IN_THREAD, event, false, was_grabbed);
}

void generic_windowObj::handlerObj
::do_button_event(IN_THREAD_ONLY,
		  const xcb_button_release_event_t *event,
		  bool buttonpress)
{
	do_button_event(IN_THREAD, event, buttonpress, grab_locked(IN_THREAD));
}

void generic_windowObj::handlerObj
::do_button_event(IN_THREAD_ONLY,
		  const xcb_button_release_event_t *event,
		  bool buttonpress,
		  bool was_grabbed)
{
	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	button_event be{event->state, keysyms, event->detail, buttonpress};

	motion_event me{be, motion_event_type::button_event,
			event->event_x, event->event_y};

	report_pointer_xy(IN_THREAD, me, was_grabbed);

	// report_pointer_xy() might not always set
	// most_recent_element_with_pointer(IN_THREAD).

	if (most_recent_element_with_pointer(IN_THREAD) &&
	    !most_recent_element_with_pointer(IN_THREAD)
	    ->process_button_event(IN_THREAD, be, event->time)

	    // Clicking pointer button 1 nowhere in particular removes keyboard
	    // focus from anything that might have it, right now.
	    && be.button == 1 && buttonpress)
		unset_keyboard_focus(IN_THREAD);
}

void generic_windowObj::handlerObj::grab(IN_THREAD_ONLY,
					 const ref<elementObj::implObj> &e)
{
	set_element_with_pointer(IN_THREAD, e);
	if (most_recent_element_with_pointer(IN_THREAD) &&
	    grabbed_timestamp(IN_THREAD) != XCB_CURRENT_TIME &&
	    !grab_locked(IN_THREAD))
	{
		grab_locked(IN_THREAD)=true;
		xcb_allow_events(IN_THREAD->info->conn,
				 XCB_ALLOW_ASYNC_BOTH,
				 grabbed_timestamp(IN_THREAD));
	}
}

void generic_windowObj::handlerObj::grab(IN_THREAD_ONLY)
{
	throw EXCEPTION("Internal error: called grab() on the top level window.");
}

void generic_windowObj::handlerObj::configure_notify(IN_THREAD_ONLY,
						     const rectangle &r)
{
	{
		mpobj<rectangle>::lock lock(current_position);

		if (*lock == r)
			return;

		*lock=r;
	}
	has_exposed(IN_THREAD)=false; // Exposure event coming.

	auto new_position=element_position(r);

	if (data(IN_THREAD).current_position == new_position)
	{
		absolute_location_updated(IN_THREAD);
	}
	else
	{
		update_current_position(IN_THREAD, new_position);
	}
}

void generic_windowObj::handlerObj::current_position_updated(IN_THREAD_ONLY)
{
	schedule_update_position_processing(IN_THREAD);
}

bool generic_windowObj::handlerObj::process_key_event(IN_THREAD_ONLY,
						      const key_event &ke)
{
	if (!ke.keypress)
		return false;

	if (!ke.notspecial())
		return false;

	if (ke.keysym == XK_ISO_Left_Tab)
	{
		if (most_recent_keyboard_focus(IN_THREAD))
		{
			most_recent_keyboard_focus(IN_THREAD)->prev_focus(IN_THREAD);
			return true;
		}

		for (auto b=focusable_fields(IN_THREAD).begin(),
			     e=focusable_fields(IN_THREAD).end();
		     b != e;)
		{
			--e;
			const auto &element=*e;

			if (element->enabled(IN_THREAD))
			{
				element->set_focus_and_ensure_visibility(IN_THREAD);
				return true;
			}
		}
	}

	if (ke.unicode == '\t')
	{
		if (most_recent_keyboard_focus(IN_THREAD))
		{
			most_recent_keyboard_focus(IN_THREAD)->next_focus(IN_THREAD);
			return true;
		}

		for (const auto &element:focusable_fields(IN_THREAD))
			if (element->enabled(IN_THREAD))
			{
				element->set_focus_and_ensure_visibility(IN_THREAD);
				return true;
			}
	}
	return false;
}

void generic_windowObj::handlerObj::unset_keyboard_focus(IN_THREAD_ONLY)
{
	if (most_recent_keyboard_focus(IN_THREAD))
	{
		focusable_impl f=most_recent_keyboard_focus(IN_THREAD);

		most_recent_keyboard_focus(IN_THREAD)=nullptr;

		f->get_focusable_element()
			.lose_focus(IN_THREAD,
				    &elementObj::implObj
				    ::report_keyboard_focus);
	}

	// Notify the XIM server that we do not have input focus.

	with_xim_client([&]
			(const auto &client)
			{
				client->focus_state(IN_THREAD, false);
			});
}

void generic_windowObj::handlerObj
::set_keyboard_focus_to(IN_THREAD_ONLY, const focusable_impl &element)
{
	auto old_focus=most_recent_keyboard_focus(IN_THREAD);

	most_recent_keyboard_focus(IN_THREAD)=element;

	auto &e=element->get_focusable_element();

	e.request_focus(IN_THREAD, old_focus,
			&elementObj::implObj::report_keyboard_focus);

	// Update the XIM server.

	// uses_input_method() gets translated to an indication to the XIM
	// server whether we have the input focus or not. If the display
	// element does not use the input method, the XIM server is informed
	// that we do not have input focus. If the display element will use
	// an input method, we notify the XIM server that we have input
	// focus.

	with_xim_client([&]
			(const auto &client)
			{
				client->focus_state(IN_THREAD,
						    e.uses_input_method());
			});
}

void generic_windowObj::handlerObj
::pointer_motion_event(IN_THREAD_ONLY,
		       const xcb_motion_notify_event_t *event)
{
	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	input_mask mask{event->state, keysyms};

	motion_event me{mask, motion_event_type::real_motion,
			event->event_x, event->event_y};

	report_pointer_xy(IN_THREAD, me);
}

void generic_windowObj::handlerObj
::enter_notify_event(IN_THREAD_ONLY,
		     const xcb_enter_notify_event_t *event)
{
	// Treat it just as any other pointer motion event

	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	input_mask mask{event->state, keysyms};

	motion_event me{mask, motion_event_type::enter_event,
			event->event_x, event->event_y};
	report_pointer_xy(IN_THREAD, me);
}

void generic_windowObj::handlerObj
::leave_notify_event(IN_THREAD_ONLY,
		     const xcb_leave_notify_event_t *event)
{
	pointer_focus_lost(IN_THREAD);
}

void generic_windowObj::handlerObj
::focus_change_event(IN_THREAD_ONLY, bool flag)
{
	has_focus(IN_THREAD)=flag;
	if (most_recent_keyboard_focus(IN_THREAD))
		most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
			.window_focus_change(IN_THREAD, flag);
}

void generic_windowObj::handlerObj
::report_pointer_xy(IN_THREAD_ONLY,
		    motion_event &me)
{
	report_pointer_xy(IN_THREAD, me, grab_locked(IN_THREAD));
}

void generic_windowObj::handlerObj
::report_pointer_xy(IN_THREAD_ONLY,
		    motion_event &me,
		    bool was_grabbed)
{
	auto g=most_recent_element_with_pointer(IN_THREAD);
	// If was_grabbed, this element passively grabbed the pointer.
	//
	// We also need to check for active grabs, which take precedence:

	auto pg=current_pointer_grab(IN_THREAD).getptr();

	if (pg)
	{
		auto grabbing_element=pg->grabbing_element.getptr();

		if (grabbing_element)
		{
			g=grabbing_element;
			was_grabbed=true;
		}
	}

	if (was_grabbed && g)
	{
		auto position=g->get_absolute_location(IN_THREAD);

		// Compute coordinates directly.

		me.x=coord_t::truncate((coord_squared_t::value_type)
				       coord_t::value_type(me.x)
				       -coord_t::value_type
				       (position.x));
		me.y=coord_t::truncate((coord_squared_t::value_type)
				       coord_t::value_type(me.y)
				       -coord_t::value_type
				       (position.y));

		g->report_motion_event(IN_THREAD, me);
		return;
	}
	// Locate the lowermost visible element for the given position.

	ref<elementObj::implObj> e{this};

	bool found;

	do
	{
		found=false;

		e->for_each_child(IN_THREAD,
				  [&]
				  (const auto &child_element)
				  {
					  auto child=child_element->impl;

					  if (found)
						  return;

					  // Ignore zombies!

					  if (child->data(IN_THREAD).removed)
						  return;

					  if (!child->data(IN_THREAD)
					      .inherited_visibility)
						  return;

					  const auto &p=child->data(IN_THREAD)
						  .current_position;

					  if (!p.overlaps(me.x, me.y))
						  return;

					  found=true;
					  e=child;

					  me.x=coord_t::truncate(me.x-p.x);
					  me.y=coord_t::truncate(me.y-p.y);
				  });
	} while (found);

	set_element_with_pointer(IN_THREAD, e);

	if (most_recent_element_with_pointer(IN_THREAD))
		most_recent_element_with_pointer(IN_THREAD)
			->report_motion_event(IN_THREAD, me);
}

void generic_windowObj::handlerObj
::set_element_with_pointer(IN_THREAD_ONLY, const ref<elementObj::implObj> &e)
{
	// Even though we checked the "removed" flag, already, someday someone
	// may win the lottery and we end up here when the top level
	// main_window gets removed. The whole purpose of the removed flag
	// is to avoid circular references.

	if (e->data(IN_THREAD).removed)
	{
		pointer_focus_lost(IN_THREAD);
		return;
	}

	if (most_recent_element_with_pointer(IN_THREAD) != e)
	{
		auto old=most_recent_element_with_pointer(IN_THREAD);
		most_recent_element_with_pointer(IN_THREAD)=e;

		e->request_focus(IN_THREAD, old,
				 &elementObj::implObj::report_pointer_focus);
	}
}

void generic_windowObj::handlerObj::removing_element(IN_THREAD_ONLY,
						     const ref<elementObj
						     ::implObj> &ei)
{
	// The container should've done this, but we'll do it just in case,
	// too...
	ei->removed_from_container(IN_THREAD);

	if (most_recent_element_with_pointer(IN_THREAD) == ei)
	{
		grab_locked(IN_THREAD)=false;
		release_grabs(IN_THREAD);
		pointer_focus_lost(IN_THREAD);
	}
}

void generic_windowObj::handlerObj::pointer_focus_lost(IN_THREAD_ONLY)
{
	if (grab_locked(IN_THREAD))
		return;

	auto cpy=most_recent_element_with_pointer(IN_THREAD);

	if (cpy.null())
		return;

	most_recent_element_with_pointer(IN_THREAD)=nullptr;
	cpy->lose_focus(IN_THREAD,
			&elementObj::implObj::report_pointer_focus);
}

void generic_windowObj::handlerObj::update_frame_extents(IN_THREAD_ONLY)
{
	auto &data=frame_extents(IN_THREAD);

	auto old_data=data;

	data.workarea=get_screen()->get_workarea();
	{
		mpobj<ewmh>::lock lock(screenref->get_connection()->impl
				       ->ewmh_info);

		lock->get_frame_extents(data.left,
					data.right,
					data.top,
					data.bottom,
					id());
	}

	if (old_data == data)
		return;

	frame_extents_updated(IN_THREAD);
}

void generic_windowObj::handlerObj::frame_extents_updated(IN_THREAD_ONLY)
{
}

static const struct {
	const char *n;
	xcb_atom_t xcb_ewmh_connection_t::*atom;
} window_type_atoms[]={
	{"desktop", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DESKTOP},
	{"dock", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DOCK},
	{"toolbar", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_TOOLBAR},
	{"menu", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_MENU},
	{"utility", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_UTILITY},
	{"splash", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_SPLASH},
	{"dialog", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DIALOG},
	{"dropdown_menu",
	 &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU},
	{"popup_menu", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_POPUP_MENU},
	{"tooltip", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_TOOLTIP},
	{"notification",
	 &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_NOTIFICATION},
	{"combo", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_COMBO},
	{"dnd", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_DND},
	{"normal", &xcb_ewmh_connection_t::_NET_WM_WINDOW_TYPE_NORMAL},
};

void generic_windowObj::handlerObj::set_window_type(const std::string &s)
{
	IN_THREAD->run_as
		(RUN_AS,
		 [s,
		  connection_impl=screenref->get_connection()->impl,
		  me=ref<generic_windowObj::handlerObj>(this)]
		 (IN_THREAD_ONLY)
		 {
			 mpobj<ewmh>::lock lock(connection_impl->ewmh_info);

			 if (!lock->ewmh_available)
				 return;

			 std::vector<xcb_atom_t> atoms;

			 xcb_ewmh_connection_t *ewmh_conn=&*lock;

			 std::list<std::string> words;

			 strtok_str(s, ", \t\r\n", words);

			 for (auto &w:words)
			 {
				 std::transform(w.begin(), w.end(), w.begin(),
						chrcasecmp::tolower);

				 for (auto &a:window_type_atoms)
				 {
					 if (w == a.n)
					 {
						 atoms.push_back(ewmh_conn
								 ->*a.atom);
						 break;
					 }
				 }
			 }

			 if (atoms.empty())
				 return;

			 xcb_ewmh_set_wm_window_type(ewmh_conn,
						     me->id(),
						     atoms.size(),
						     &atoms[0]);
		 });
}

void generic_windowObj::handlerObj
::do_update_wm_hints(const function<update_wm_hints_t>  &callback)
{
	auto c=screenref->get_connection()->impl->info->conn;

	returned_pointer<xcb_generic_error_t *> error;

	auto return_value=return_pointer
		(xcb_get_property_reply
		 (c,
		  xcb_icccm_get_wm_hints(c, id()),
		  error.addressof()));

	if (error)
		throw EXCEPTION(connection_error(error));

	xcb_icccm_wm_hints_t hints=xcb_icccm_wm_hints_t();

	xcb_icccm_get_wm_hints_from_reply(&hints, return_value);

	callback(hints);

	xcb_icccm_set_wm_hints(c, id(), &hints);
}

////////////////////////////////////////////////////////////////////
//
// Inherited from drawableObj::implObj

screen generic_windowObj::handlerObj::get_screen()
{
	return screenref;
}

const_screen generic_windowObj::handlerObj::get_screen() const
{
	return screenref;
}

dim_t generic_windowObj::handlerObj::get_width() const
{
	return mpobj<rectangle>::lock(current_position)->width;
}

dim_t generic_windowObj::handlerObj::get_height() const
{
	return mpobj<rectangle>::lock(current_position)->width;
}

void generic_windowObj::handlerObj
::set_window_title(const std::experimental::string_view &s)
{
	IN_THREAD->run_as
		(RUN_AS,
		 [title=std::string{s},
		  connection_impl=screenref->get_connection()->impl,
		  me=ref<generic_windowObj::handlerObj>(this)]
		 (IN_THREAD_ONLY)
		 {
			 mpobj<ewmh>::lock lock(connection_impl->ewmh_info);

			 if (lock->ewmh_available)
				 xcb_ewmh_set_wm_name(&*lock,
						      me->id(),
						      title.size(),
						      title.c_str());
			 else
				 xcb_icccm_set_wm_name(IN_THREAD->info->conn,
						       me->id(),
						       XCB_ATOM_STRING, 8,
						       title.size(),
						       title.c_str());
		 });
}

void generic_windowObj::handlerObj::paste(IN_THREAD_ONLY, xcb_atom_t clipboard,
					  xcb_timestamp_t timestamp)
{
	incremental_conversion_in_progress=false;

	convert_selection(IN_THREAD, clipboard,
			  IN_THREAD->info->atoms_info.utf8_string,
			  timestamp);
}

void generic_windowObj::handlerObj
::conversion_failed(IN_THREAD_ONLY, xcb_atom_t clipboard,
		    xcb_atom_t type,
		    xcb_timestamp_t timestamp)
{
	if (type == IN_THREAD->info->atoms_info.utf8_string)
		convert_selection(IN_THREAD, clipboard,
				  IN_THREAD->info->atoms_info.string,
				  timestamp);
}

bool generic_windowObj::handlerObj
::begin_converted_data(IN_THREAD_ONLY, xcb_atom_t type,
		       xcb_timestamp_t timestamp)
{
	if (type == IN_THREAD->info->atoms_info.string)
	{
		if (!incremental_conversion_in_progress)
		{
			unicode::iconvert::tou::end();
			unicode::iconvert::tou::begin(unicode::iso_8859_1);
		}
	}
	else if (type == IN_THREAD->info->atoms_info.utf8_string)
	{
		if (!incremental_conversion_in_progress)
		{
			unicode::iconvert::tou::end();
			unicode::iconvert::tou::begin(unicode::utf_8);
		}
	}
	else
	{
		if (type == IN_THREAD->info->atoms_info.net_frame_extents)
			update_frame_extents(IN_THREAD);
		return false;
	}
	received_converted_data=false;
	return true;
}

void generic_windowObj::handlerObj
::converting_incrementally(IN_THREAD_ONLY,
			   xcb_atom_t type,
			   xcb_timestamp_t timestamp,
			   uint32_t estimated_size)
{
	incremental_conversion_in_progress=true;
	received_converted_data=true;
}

void generic_windowObj::handlerObj
::converted_data(IN_THREAD_ONLY, xcb_atom_t clipboard,
		 xcb_atom_t type,
		 xcb_atom_t format,
		 void *data,
		 size_t size)
{
	received_converted_data=true;
	unicode::iconvert::tou::operator()
		(reinterpret_cast<char *>(data), size);
}

void generic_windowObj::handlerObj
::end_converted_data(IN_THREAD_ONLY, xcb_atom_t clipboard,
				xcb_timestamp_t timestamp)
{
	if (!received_converted_data)
		incremental_conversion_in_progress=false;

	if (!incremental_conversion_in_progress)
		unicode::iconvert::tou::end();
}

int generic_windowObj::handlerObj::converted(const char32_t *ptr, size_t cnt)
{
	try {
		// This is called from the operator(), this is cool.
		pasted_string(thread(), {ptr, cnt});
	} CATCH_EXCEPTIONS;
	return 0;
}

void generic_windowObj::handlerObj
::pasted_string(IN_THREAD_ONLY, const std::experimental::u32string_view &s)
{
	if (most_recent_keyboard_focus(IN_THREAD))
		most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
			.pasted(IN_THREAD, s);
}

LIBCXXW_NAMESPACE_END
