/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "pictformat.H"
#include "draw_info.H"
#include "draw_info_cache.H"
#include "container_element.H"
#include "layoutmanager.H"
#include "screen.H"
#include "xid_t.H"
#include "element_screen.H"
#include "background_color.H"
#include "focus/focusable.H"
#include "x/w/key_event.H"
#include "x/w/values_and_mask.H"
#include <xcb/xcb_icccm.h>
#include <X11/keysym.h>
#include "child_element.H"

LIBCXXW_NAMESPACE_START

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
//
// Allocate a picture for the input/output window

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
	current_background_color_thread_only(default_background_color
					     (params.window_handler_params
					      .screenref))
{
}

generic_windowObj::handlerObj::~handlerObj()=default;

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
		xcb_map_window(IN_THREAD->info->conn, id());
		visibility_info.do_not_redraw=true;
	}
	else
	{
		xcb_unmap_window(IN_THREAD->info->conn, id());
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

void generic_windowObj::handlerObj::request_visibility(IN_THREAD_ONLY,
						       bool flag)
{
	if (!flag || preferred_dimensions_set(IN_THREAD))
	{
		elementObj::implObj::request_visibility(IN_THREAD, flag);
		return;
	}

	// Don't do this immediately. Schedule a thread job to do this, after
	// everything shakes down.

	IN_THREAD->run_as
		(RUN_AS,
		 [me=ref<generic_windowObj::handlerObj>(this)]
		 (IN_THREAD_ONLY)
		 {
			 me->preferred_dimensions_set(IN_THREAD)=true;

			 // An X server will report a BadValue for a window
			 // of size (0,0). Our metrics can compute such
			 // a preferred size, as an edge case. So, we deal
			 // with it.

			 auto w=me->preferred_width(IN_THREAD);
			 auto h=me->preferred_height(IN_THREAD);

			 if (w == 0 || h == 0)
				 w=h=1;

			 values_and_mask configure_window_vals
				 (XCB_CONFIG_WINDOW_WIDTH,
				  (dim_t::value_type)w,

				  XCB_CONFIG_WINDOW_HEIGHT,
				  (dim_t::value_type)h);

				 ;
#ifdef REQUEST_VISIBILITY_LOG
			 REQUEST_VISIBILITY_LOG(me->preferred_width(IN_THREAD),
						me->preferred_height(IN_THREAD)
						);
#endif
			 xcb_configure_window(IN_THREAD->info->conn, me->id(),
					      configure_window_vals.mask(),
					      configure_window_vals.values()
					      .data());

			 // Also simulate a configure_notify(), so that the
			 // display elements get arranged accordingly right
			 // away. We don't know when we'll get back the
			 // ConfigureNotify message from the display server.

			 auto r=*mpobj<rectangle>::lock(me->current_position);

			 r.width=me->preferred_width(IN_THREAD);
			 r.height=me->preferred_height(IN_THREAD);

			 me->configure_notify(IN_THREAD, r);

			 // Ok, need to wait for the reconfiguration to shake
			 // itself out too, so schedule another job to finally
			 // nail this coffin shut.

			 IN_THREAD->run_as
				 (RUN_AS,
				  [me]
				  (IN_THREAD_ONLY)
				  {
					  me->request_visibility(IN_THREAD,
								 true);
				  });
		 });
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
	exposure_event_recursive(IN_THREAD, areas);
}

void generic_windowObj::handlerObj::theme_updated_event(IN_THREAD_ONLY)
{
	// container_element_overrides_decl hijacks theme_updated(), so we
	// just do this here.

	current_background_color(IN_THREAD)->theme_updated(IN_THREAD);
	theme_updated(IN_THREAD);
}

void generic_windowObj::handlerObj
::key_press_event(IN_THREAD_ONLY,
		  const xcb_key_press_event_t *event,
		  uint16_t sequencehi)
{
	// We grab_button()ed and grab_key()ed.
	// Make sure we'll release the grab, when the dust settles.

	grabbed_timestamp(IN_THREAD)=event->time;

	do_key_event(IN_THREAD, event, sequencehi, true);
}

void generic_windowObj::handlerObj
::key_release_event(IN_THREAD_ONLY,
		    const xcb_key_release_event_t *event,
		    uint16_t sequencehi)
{
	do_key_event(IN_THREAD, event, sequencehi, false);
}

void generic_windowObj::handlerObj
::do_key_event(IN_THREAD_ONLY,
	       const xcb_key_release_event_t *event,
	       uint16_t sequencehi,
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

	if (current_focus(IN_THREAD))
		current_focus(IN_THREAD)->get_focusable_element()
			.process_key_event(IN_THREAD, ke);
	else
		process_key_event(IN_THREAD, ke);
}

void generic_windowObj::handlerObj
::button_press_event(IN_THREAD_ONLY,
		     const xcb_button_press_event_t *event)
{
	// We grab_button()ed and grab_key()ed.
	// Make sure we'll release the grab, when the dust settles.

	grabbed_timestamp(IN_THREAD)=event->time;
	button_event(IN_THREAD, event, true);
}

void generic_windowObj::handlerObj
::button_release_event(IN_THREAD_ONLY,
		       const xcb_button_release_event_t *event)
{
	button_event(IN_THREAD, event, false);
}

void generic_windowObj::handlerObj
::button_event(IN_THREAD_ONLY,
	       const xcb_button_release_event_t *event,
	       bool buttonpress)
{
	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	input_mask mask{event->state, keysyms};

	report_pointer_xy(IN_THREAD, event->event_x, event->event_y);

	// report_pointer_xy() might not always set
	// most_recent_element_with_pointer(IN_THREAD).

	if (most_recent_element_with_pointer(IN_THREAD) &&
	    !most_recent_element_with_pointer(IN_THREAD)
	    ->process_button_event(IN_THREAD, event->detail, buttonpress, mask)
	    && event->detail == 1 && buttonpress &&
	    current_focus(IN_THREAD))
	{
		current_focus(IN_THREAD)->remove_focus(IN_THREAD);
	}
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

	update_current_position(IN_THREAD,
				element_position(r));
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
		if (current_focus(IN_THREAD))
		{
			current_focus(IN_THREAD)->prev_focus(IN_THREAD);
			return true;
		}

		for (auto b=focusable_fields(IN_THREAD).begin(),
			     e=focusable_fields(IN_THREAD).end();
		     b != e;)
		{
			--e;
			const auto &element=*e;

			if (element->is_enabled(IN_THREAD))
			{
				current_focus(IN_THREAD)=element;
				element->get_focusable_element()
					.request_focus(IN_THREAD,
						       ptr<elementObj::implObj>
						       (),
						       &elementObj::implObj
						       ::report_keyboard_focus);
				return true;
			}
		}
	}

	if (ke.unicode == '\t')
	{
		if (current_focus(IN_THREAD))
		{
			current_focus(IN_THREAD)->next_focus(IN_THREAD);
			return true;
		}

		for (const auto &element:focusable_fields(IN_THREAD))
			if (element->is_enabled(IN_THREAD))
			{
				current_focus(IN_THREAD)=element;
				element->get_focusable_element()
					.request_focus(IN_THREAD,
						       ptr<elementObj::implObj>
						       (),
						       &elementObj::implObj
						       ::report_keyboard_focus);
				return true;
			}
	}
	return false;
}

void generic_windowObj::handlerObj
::pointer_motion_event(IN_THREAD_ONLY,
		       const xcb_motion_notify_event_t *event)
{
	report_pointer_xy(IN_THREAD, event->event_x, event->event_y);
}

void generic_windowObj::handlerObj
::enter_notify_event(IN_THREAD_ONLY,
		     const xcb_enter_notify_event_t *event)
{
	// Treat it just as any other pointer motion event

	report_pointer_xy(IN_THREAD, event->event_x, event->event_y);
}

void generic_windowObj::handlerObj
::leave_notify_event(IN_THREAD_ONLY,
		     const xcb_leave_notify_event_t *event)
{
	pointer_focus_lost(IN_THREAD);
}

void generic_windowObj::handlerObj
::report_pointer_xy(IN_THREAD_ONLY,
			       coord_t x,
			       coord_t y)
{
	// Locate the lowermost element for the given position.

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

					  const auto &p=child->data(IN_THREAD)
						  .current_position;

					  if (!p.overlaps(x, y))
						  return;

					  found=true;
					  e=child;

					  x=coord_t::truncate(x-p.x);
					  y=coord_t::truncate(y-p.y);
				  });
	} while (found);

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
		pointer_focus_lost(IN_THREAD);
}

void generic_windowObj::handlerObj::pointer_focus_lost(IN_THREAD_ONLY)
{
	auto cpy=most_recent_element_with_pointer(IN_THREAD);

	if (cpy.null())
		return;

	most_recent_element_with_pointer(IN_THREAD)=nullptr;
	cpy->lose_focus(IN_THREAD,
			&elementObj::implObj::report_pointer_focus);
}

bool generic_windowObj::handlerObj::get_frame_extents(dim_t &left,
						      dim_t &right,
						      dim_t &top,
						      dim_t &bottom) const
{
	mpobj<ewmh>::lock lock(screenref->get_connection()
			       ->impl->ewmh_info);

	return lock->get_frame_extents(left, right, top, bottom,
				       screenref->impl->screen_number,
				       id());
}

void generic_windowObj::handlerObj::horizvert_updated(IN_THREAD_ONLY)
{
	auto conn=screenref->get_connection()->impl;

	auto p=get_horizvert(IN_THREAD);

	auto minimum_width=p->horiz.minimum();
	auto minimum_height=p->vert.minimum();
	auto new_preferred_width=p->horiz.preferred();
	auto new_preferred_height=p->vert.preferred();

	// Don't tell the window manager that our minimum
	// dimensions exceed usable workarea size.

	// Subtract frame size from total workarea size.
	// That's our cap.

	dim_t left, right, top, bottom;

	auto workarea=screenref->get_workarea();

	if (get_frame_extents(left, right, top, bottom))
	{
		auto usable_workarea_width=
			workarea.width-left-right;

		auto usable_workarea_height=
			workarea.height-top-bottom;

		if (usable_workarea_width < minimum_width)
			minimum_width=usable_workarea_width;

		if (usable_workarea_height < minimum_height)
			minimum_height=usable_workarea_height;

		if (usable_workarea_width < new_preferred_width)
			new_preferred_width=usable_workarea_width;

		if (usable_workarea_height < new_preferred_height)
			new_preferred_height=usable_workarea_height;
	}

	preferred_width(IN_THREAD)=new_preferred_width;
	preferred_height(IN_THREAD)=new_preferred_height;

	xcb_size_hints_t hints=xcb_size_hints_t();

	xcb_icccm_size_hints_set_min_size(&hints,
					  (dim_t::value_type)minimum_width,
					  (dim_t::value_type)minimum_height);
	xcb_icccm_size_hints_set_base_size(&hints,
					   (dim_t::value_type)new_preferred_width,
					   (dim_t::value_type)new_preferred_height);
	xcb_icccm_size_hints_set_max_size(&hints,
					  (dim_t::value_type)p->horiz.maximum(),
					  (dim_t::value_type)p->vert.maximum());

	xcb_icccm_set_wm_size_hints(conn->info->conn,
				    id(),
				    conn->info->atoms_info.wm_normal_hints,
				    &hints);
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

LIBCXXW_NAMESPACE_END
