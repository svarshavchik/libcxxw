/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "shared_handler_data.H"
#include "connection_thread.H"
#include "connectionfwd.H"
#include "defaulttheme.H"
#include "pictformat.H"
#include "icon.H"
#include "icon_images_set_element.H"
#include "cursor_pointer_element.H"
#include "draw_info.H"
#include "container_element.H"
#include "layoutmanager.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "xid_t.H"
#include "element_screen.H"
#include "background_color.H"
#include "background_color_element.H"
#include "cursor_pointer.H"
#include "focus/focusable.H"
#include "grabbed_pointer.H"
#include "xim/ximclient.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/values_and_mask.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/main_window.H"
#include "child_element.H"
#include "hotspot.H"
#include "shortcut/installed_shortcut.H"
#include "catch_exceptions.H"
#include <x/property_value.H>
#include <x/weakcapture.H>
#include <x/pidinfo.H>
#include <courier-unicode.h>
#include <xcb/xcb_icccm.h>
#include <string>

LIBCXXW_NAMESPACE_START

static property::value<bool> disable_grabs(LIBCXX_NAMESPACE_STR
					   "::w::disable_grab", false);

static property::value<unsigned> double_click(LIBCXX_NAMESPACE_STR
					      "::w::double_click", 1000);

static rectangle element_position(const rectangle &r)
{
	auto cpy=r;

	cpy.x=0;
	cpy.y=0;
	return cpy;
}

static background_color default_background_color(const screen &s,
						 const color_arg &color)
{
	return s->impl->create_background_color(color);
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
create_constructor_params(const screen &parent_screen,
			  const color_arg &background_color,
			  size_t nesting_level)
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
		parent_screen->impl->toplevelwindow_pictformat,
		nesting_level,
		background_color
	};
}

generic_windowObj::handlerObj
::handlerObj(ONLY IN_THREAD,
	     const screen &parent_screen,
	     const color_arg &background_color,
	     const shared_handler_data &handler_data,
	     size_t nesting_level)
	: handlerObj(IN_THREAD, handler_data,
		     create_constructor_params(parent_screen, background_color,
					       nesting_level))
{
}

generic_windowObj::handlerObj
::handlerObj(ONLY IN_THREAD,
	     const shared_handler_data &handler_data,
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
	// And now, the element that represents the window itself, and
	// all the theme-based resources: background colors, icons, and
	// masks
	superclass_t{
	default_background_color(params.window_handler_params.screenref,
				 params.background_color),
		default_background_color(params.window_handler_params.screenref,
					 "modal_shade"),
		drawableObj::implObj::create_icon(create_icon_args_t{
				"disabled_mask",
					render_repeat::normal},
			params.window_handler_params.screenref),
		cursor_pointer::create
		(drawableObj::implObj::create_icon(create_icon_args_t{
				"cursor-wait"},
			params.window_handler_params.screenref)),
		params.nesting_level,
		element_position(params.window_handler_params.initial_position),
		params.window_handler_params.screenref,
		params.drawable_pictformat,
		"background@libcxx.com"},
	current_events_thread_only{(xcb_event_mask_t)
			params.window_handler_params
			.events_and_mask.m.at(XCB_CW_EVENT_MASK)},
	current_position{params.window_handler_params.initial_position},
	handler_data{handler_data},
	original_background_color{params.background_color},
	frame_extents_thread_only{params.window_handler_params.screenref
			->get_workarea()},
	current_theme_thread_only{params.window_handler_params.screenref
			->impl->current_theme.get()}
{
	char hostnamebuf[256];

	if (gethostname(hostnamebuf, sizeof(hostnamebuf)))
		LOG_ERROR("gethostname() failed");
	else
	{
		hostnamebuf[sizeof(hostnamebuf)-1]=0;

		xcb_icccm_set_wm_client_machine(IN_THREAD->info->conn,
						id(),
						XCB_ATOM_STRING,
						8,
						strlen(hostnamebuf),
						hostnamebuf);
	}

	mpobj<ewmh>::lock lock{screenref->get_connection()->impl->ewmh_info};

	lock->set_window_pid(id());
	lock->set_user_time_window(id(), id());
}

generic_windowObj::handlerObj::~handlerObj()=default;

void generic_windowObj::handlerObj::installed(ONLY IN_THREAD)
{
	{
		mpobj<ewmh>::lock lock{screenref->get_connection()
				->impl->ewmh_info};

		lock->request_frame_extents(screenref->impl->screen_number, id());
	}

	// The top level window is not a child element in a container,
	// so it is, hereby, initialized!

	initialize_if_needed(IN_THREAD);
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

draw_info &generic_windowObj::handlerObj::get_draw_info(ONLY IN_THREAD)
{
	if (data(IN_THREAD).cached_draw_info)
		return *data(IN_THREAD).cached_draw_info;

	return get_draw_info_from_scratch(IN_THREAD);
}

draw_info &generic_windowObj::handlerObj::get_draw_info_from_scratch(ONLY IN_THREAD)
{
	auto &viewport=data(IN_THREAD).current_position;

	return *(data(IN_THREAD).cached_draw_info={
			viewport,
			{viewport}, // No parent, everything is visible.
			current_background_color(IN_THREAD)
				->get_current_color(IN_THREAD)
				->impl,
			0,
			0,
				});

}

void generic_windowObj::handlerObj
::set_background_color(ONLY IN_THREAD,
		       const background_color &c)
{
	auto b=current_background_color(IN_THREAD);

	background_color_element<background_color_tag>::update(IN_THREAD, c);

	// We must check AFTER the update, because this background color
	// may be a gradient that's adjusted according to our size.
	if (current_background_color(IN_THREAD) == b)
		return;

	// Background color changed (1/2).
	background_color_changed(IN_THREAD);
}

background_color generic_windowObj::handlerObj
::current_background_color(ONLY IN_THREAD)
{
	return background_color_element<background_color_tag>::get(IN_THREAD);
}

const background_color generic_windowObj::handlerObj
::shaded_color(ONLY IN_THREAD)
{
	return background_color_element<shaded_color_tag>::get(IN_THREAD);
}

const icon generic_windowObj::handlerObj::disabled_mask(ONLY IN_THREAD)
{
	return icon_1tag<disabled_mask_tag>::tagged_icon(IN_THREAD);
}

rectangle generic_windowObj::handlerObj
::get_absolute_location(ONLY IN_THREAD)
{
	return data(IN_THREAD).current_position;
}

void generic_windowObj::handlerObj
::get_absolute_location_on_screen(ONLY IN_THREAD, rectangle &r)
{
	r.x=coord_t::truncate(r.x + root_x(IN_THREAD));
	r.y=coord_t::truncate(r.y + root_y(IN_THREAD));
}

void generic_windowObj::handlerObj
::add_root_xy(ONLY IN_THREAD, coord_t &x, coord_t &y)
{
	x=coord_t::truncate(x + root_x(IN_THREAD));
	y=coord_t::truncate(y + root_y(IN_THREAD));
}

void generic_windowObj::handlerObj
::subtract_root_xy(ONLY IN_THREAD, coord_t &x, coord_t &y)
{
	x=coord_t::truncate(coord_t::truncate(x) -
			    coord_t::truncate(root_x(IN_THREAD)));

	y=coord_t::truncate(coord_t::truncate(y) -
			    coord_t::truncate(root_y(IN_THREAD)));
}

void generic_windowObj::handlerObj
::draw_child_elements_after_visibility_updated(ONLY IN_THREAD, bool flag)
{
}

void generic_windowObj::handlerObj
::set_inherited_visibility(ONLY IN_THREAD,
			   inherited_visibility_info &visibility_info)
{
	if (visibility_info.flag)
	{
		visibility_info.do_not_redraw=true;

		// Need to delay mapping until the connection thread
		// is completely idle. Recursive call to request_visibility()
		// may get short-circuited in update_visibility() bailing out
		// if !initialized, that gets rescheduled after some pending
		// connection thread callback finally initializes the display
		// element, so in order for mapping to work, we need to make
		// sure all of this settles down before mapping.

		IN_THREAD->idle_callbacks(IN_THREAD)->push_back
			([me=make_weak_capture(ref(this))]
			 (ONLY IN_THREAD)
			 {
				 auto got=me.get();

				 if (!got)
					 return;

				 auto &[me]=*got;

				 if (!me->data(IN_THREAD).inherited_visibility)
					 return; // The show is cancelled

				 if (me->is_really_mapped)
					 return; // Definitely cancelled

				 me->is_really_mapped=true;
				 me->set_inherited_visibility_mapped(IN_THREAD);
			 });
	}
	else
	{
		if (is_really_mapped)
		{
			is_really_mapped=false;
			set_inherited_visibility_unmapped(IN_THREAD);
		}
	}

	superclass_t::set_inherited_visibility(IN_THREAD, visibility_info);
}

std::string
generic_windowObj::handlerObj::default_wm_class_resource(ONLY IN_THREAD)
{
	auto n=exename();

	size_t p=n.rfind('/');

	if (p != n.npos)
		n=n.substr(++p);
	return n;
}

void generic_windowObj::handlerObj
::set_inherited_visibility_mapped(ONLY IN_THREAD)
{
	// We establish passive grabs for any button or keypress.

	// button_press_event() and key_press_event() will take care of
	// releasing the grabs.

#ifdef MAP_LOG
	MAP_LOG();
#endif
	if (!disable_grabs.get())
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

	// Set WM_CLASS before mapping the window.

	if (wm_class_resource(IN_THREAD).empty())
		wm_class_resource(IN_THREAD)=
			default_wm_class_resource(IN_THREAD);

	if (wm_class_instance(IN_THREAD).empty())
		wm_class_instance(IN_THREAD)=
			default_wm_class_instance();

	{
		std::vector<char> instance_resource;

		instance_resource.reserve(wm_class_instance(IN_THREAD)
					  .size()+
					  wm_class_resource(IN_THREAD)
					  .size()+2);

		instance_resource.insert(instance_resource.end(),
					 wm_class_instance(IN_THREAD)
					 .begin(),
					 wm_class_instance(IN_THREAD)
					 .end());
		instance_resource.push_back(0);

		instance_resource.insert(instance_resource.end(),
					 wm_class_resource(IN_THREAD)
					 .begin(),
					 wm_class_resource(IN_THREAD)
					 .end());
		instance_resource.push_back(0);

		xcb_icccm_set_wm_class(IN_THREAD->info->conn,
				       id(),
				       instance_resource.size(),
				       &*instance_resource.begin());
	}

	xcb_map_window(IN_THREAD->info->conn, id());

	// Find the first element with autofocus(), and make it so.

	for (const auto &element:focusable_fields(IN_THREAD))
	{
		if (!element->focusable_enabled(IN_THREAD))
			continue;

		if (!element->autofocus.get())
			continue;

		element->set_focus_and_ensure_visibility(IN_THREAD, {});
		break;
	}
}

void generic_windowObj::handlerObj
::set_inherited_visibility_unmapped(ONLY IN_THREAD)
{
	xcb_unmap_window(IN_THREAD->info->conn, id());
	ungrab(IN_THREAD);
	xcb_ungrab_key(IN_THREAD->info->conn,
		       0,
		       id(),
		       XCB_MOD_MASK_ANY);

	xcb_ungrab_button(IN_THREAD->info->conn,
			  0,
			  id(),
			  XCB_MOD_MASK_ANY);
}

void generic_windowObj::handlerObj::remove_background_color(ONLY IN_THREAD)
{
	set_background_color(IN_THREAD,
			     screenref->impl->create_background_color
			     (original_background_color));
}

bool generic_windowObj::handlerObj::has_own_background_color(ONLY IN_THREAD)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Inherited from window_handler

void generic_windowObj::handlerObj::process_collected_exposures(ONLY IN_THREAD)
{
	has_exposed(IN_THREAD)=true;
	exposure_event_recursive(IN_THREAD,
				 exposure_rectangles(IN_THREAD).rectangles);
}

void generic_windowObj::handlerObj
::process_collected_graphics_exposures(ONLY IN_THREAD)
{
	exposure_event_recursive(IN_THREAD,
				 graphics_exposure_rectangles(IN_THREAD)
				 .rectangles);
}

void generic_windowObj::handlerObj::theme_updated_event(ONLY IN_THREAD)
{
	auto new_theme=get_screen()->impl->current_theme.get();

	bool is_different_theme=
		new_theme->is_different_theme(current_theme(IN_THREAD));

	// Even if they're "the same", still update our current_theme.
	// Even if they're "the same", they are different objects, as such
	// we drop our ref to the old one, thus saving a little bit of
	// memory.

	current_theme(IN_THREAD)=new_theme;

	if (is_different_theme)
		theme_updated(IN_THREAD, new_theme);
}

void generic_windowObj::handlerObj::theme_updated(ONLY IN_THREAD,
						  const defaulttheme &th)
{
	auto b=current_background_color(IN_THREAD);

	superclass_t::theme_updated(IN_THREAD, th);

	if (b == current_background_color(IN_THREAD))
		return;

	// Background color changed (2/2).
	background_color_changed(IN_THREAD);
}

// Shade mcguffin.

// The destructor schedules a redraw of the entire window, when the
// shade mcguffin gets destroyed.

class LIBCXX_HIDDEN generic_windowObj::handlerObj::busy_shadeObj
	: virtual public obj {

 public:
	const weakptr<ptr<generic_windowObj::handlerObj>> handler;

	busy_shadeObj(const ref<generic_windowObj::handlerObj> &handler)
		: handler(handler)
	{
	}

	~busy_shadeObj()
	{
		auto p=handler.getptr();

		if (!p)
			return;
		p->schedule_redraw_recursively();
	}
};

// wait mcguffin

// The destructor calls update_displayed_cursor_pointer.

class LIBCXX_HIDDEN generic_windowObj::handlerObj::busy_waitObj
	: virtual public obj {

 public:
	const weakptr<ptr<generic_windowObj::handlerObj>> handler;

	busy_waitObj(const ref<generic_windowObj::handlerObj> &handler)
		: handler(handler)
	{
	}

	~busy_waitObj()
	{
		update();
	}

	void update()
	{
		auto p=handler.getptr();

		if (!p)
			return;

		auto h=ref(&*p);

		h->thread()->run_as
			([h]
			 (ONLY IN_THREAD)
			 {
				 h->update_displayed_cursor_pointer(IN_THREAD);
			 });
	}
};

ref<obj> generic_windowObj::handlerObj::get_shade_busy_mcguffin()
{
	if (drawable_pictformat->alpha_depth == 0)
		return get_wait_busy_mcguffin();

	busy_mcguffins_t::lock lock{busy_mcguffins};

	auto p=lock->shade.getptr();

	if (p) return p;

	auto n=ref<busy_shadeObj>::create(ref(this));

	lock->shade=n;

	schedule_redraw_recursively();
	return n;
}

ref<obj> generic_windowObj::handlerObj::get_wait_busy_mcguffin()
{
	busy_mcguffins_t::lock lock{busy_mcguffins};

	auto p=lock->wait_cursor.getptr();

	if (p) return p;

	auto n=ref<busy_waitObj>::create(ref(this));

	lock->wait_cursor=n;

	n->update();
	return n;
}

bool generic_windowObj::handlerObj::is_input_busy()
{
	busy_mcguffins_t::lock lock{busy_mcguffins};

	return !!lock->shade.getptr() || !!lock->wait_cursor.getptr();
}

bool generic_windowObj::handlerObj::is_shade_busy()
{
	return !!busy_mcguffins_t::lock{busy_mcguffins}->shade.getptr();
}

bool generic_windowObj::handlerObj::is_wait_busy()
{
	return !!busy_mcguffins_t::lock{busy_mcguffins}->wait_cursor.getptr();
}

void generic_windowObj::handlerObj
::update_user_time(ONLY IN_THREAD)
{
	update_user_time(IN_THREAD, IN_THREAD->timestamp(IN_THREAD));
}

void generic_windowObj::handlerObj
::update_user_time(ONLY IN_THREAD, xcb_timestamp_t t)
{
	mpobj<ewmh>::lock lock{screenref->get_connection()->impl->ewmh_info};

	lock->set_user_time(id(), t);
}

void generic_windowObj::handlerObj
::key_press_event(ONLY IN_THREAD,
		  const xcb_key_press_event_t *event,
		  uint16_t sequencehi)
{
	ungrab(IN_THREAD);

	// We grab_button()ed and grab_key()ed.
	// Make sure we'll release the grab, when the dust settles.

	grabbed_timestamp(IN_THREAD)=event->time;

	update_user_time(IN_THREAD, event->time);

	if (is_input_busy())
		// We're busy now. Since we're grabbing all key presses this
		// can only be checked now, after the grab processing.
		return;

	forward_key_event(IN_THREAD, event, sequencehi, true);
}

void generic_windowObj::handlerObj
::key_release_event(ONLY IN_THREAD,
		    const xcb_key_release_event_t *event,
		    uint16_t sequencehi)
{
	ungrab(IN_THREAD);
	forward_key_event(IN_THREAD, event, sequencehi, false);
}

void generic_windowObj::handlerObj
::forward_key_event(ONLY IN_THREAD,
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

	// Popup has grabbed pointer and keyboard input?

	auto pg=current_pointer_grab(IN_THREAD).getptr();

	if (pg)
	{
		if (handler_data->handle_key_event(IN_THREAD, event, keypress))
			return;
	}

	handle_key_event(IN_THREAD, event, keypress);
}

bool generic_windowObj::handlerObj
::handle_key_event(ONLY IN_THREAD,
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
	{
		most_recent_keyboard_focus(IN_THREAD)
			->get_focusable_element()
			.unschedule_hover_action(IN_THREAD);

		processed=most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
			.process_key_event(IN_THREAD, ke);
	}
	else
		processed=process_key_event(IN_THREAD, ke);

	// Check for shortcuts, as the last resort

	if (processed)
		return true;

	installed_shortcutptr best_shortcut;

	if (activate_for(ke))
	{
		mpobj<shortcut_lookup_t>::lock
			lock{handler_data->installed_shortcuts};

		auto shortcuts=lock->equal_range(unicode_lc(ke.unicode));

		while (shortcuts.first != shortcuts.second)
		{
			auto p=shortcuts.first->second;

			++shortcuts.first;

			// Find the first shortcut that's enabled and matches
			// the key.

			if (!p->enabled(IN_THREAD)
			    ||
			    !p->installed_shortcut(IN_THREAD).matches(ke))
				continue;

			// If there are shortcuts for both shift-Foo and
			// shift-ctrl-Foo, make sure that shift-ctrl-Foo matches
			// the right shortcut.

			auto best_ordinal=
				p->installed_shortcut(IN_THREAD).ordinal();

			best_shortcut=p;

			// If there are any other shortcuts that also match,
			// keep only the best ordinal.

			while (shortcuts.first != shortcuts.second)
			{
				p=shortcuts.first->second;

				++shortcuts.first;

				if (!p->enabled(IN_THREAD)
				    ||
				    !p->installed_shortcut(IN_THREAD)
				    .matches(ke))
					continue;

				auto ordinal=p->installed_shortcut(IN_THREAD)
					.ordinal();

				if (ordinal < best_ordinal)
					continue;

				best_shortcut=p;
				best_ordinal=ordinal;
			}
		}
	}

	if (best_shortcut)
	{
		auto main_window=get_main_window();

		if (main_window)
			try {
				best_shortcut->activated(IN_THREAD, &ke);
			} REPORT_EXCEPTIONS(main_window);
		processed=true;
	}
	return processed;
}

void generic_windowObj::handlerObj
::button_press_event(ONLY IN_THREAD,
		     const xcb_button_press_event_t *event)
{
	ungrab(IN_THREAD);

	update_user_time(IN_THREAD, event->time);

	auto click_time=std::chrono::steady_clock::now();

	if (previous_click_time)
	{
		auto cutoff=previous_click_time.value() +
			std::chrono::milliseconds(double_click.get());

		if (click_time < cutoff)
			++click_count;
		else
			click_count=1;
	}
	else
	{
		click_count=1;
	}

	previous_click_time=click_time;

	// We grab_button()ed and grab_key()ed.
	// Make sure we'll release the grab, when the dust settles.

	grabbed_timestamp(IN_THREAD)=event->time;

	do_button_event(IN_THREAD, event, true);
}

void generic_windowObj::handlerObj
::button_release_event(ONLY IN_THREAD,
		       const xcb_button_release_event_t *event)
{
	// We need to remove all the grab first, in the case that the
	// code that processed the button release event wants to regrab
	// the pointer, for some reason. However we need to use the
	// current grab status for do_button_event(), so save it first.

	bool was_grabbed=grab_locked(IN_THREAD);
	ungrab(IN_THREAD);

	do_button_event(IN_THREAD, event, false, was_grabbed);
}

void generic_windowObj::handlerObj
::do_button_event(ONLY IN_THREAD,
		  const xcb_button_release_event_t *event,
		  bool buttonpress)
{
	do_button_event(IN_THREAD, event, buttonpress, grab_locked(IN_THREAD));
}

void generic_windowObj::handlerObj
::do_button_event(ONLY IN_THREAD,
		  const xcb_button_release_event_t *event,
		  bool buttonpress,
		  bool was_grabbed)
{
	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	button_event be{event->state, keysyms, event->detail, buttonpress,
			click_count};

	motion_event me{be, (activate_for(be)
			     ? motion_event_type::button_action_event
			     : motion_event_type::button_event),
			event->event_x, event->event_y};

	auto report_to=report_pointer_xy(IN_THREAD, me, was_grabbed);

	if (report_to->is_input_busy())
		return;

	handler_data->reporting_button_event_to(IN_THREAD, ref(this),
						report_to, be);

	report_to->do_button_event(IN_THREAD, event, be, me);

	// The passive grab was released by a button release event.
	// We need to re-report this as a motion event. If the grabbing element
	// had a custom pointer installed and the pointer is no longer in the
	// element, we need to update_displayed_cursor_pointer().

	if (was_grabbed && !buttonpress)
		report_to->report_pointer_xy(IN_THREAD, me, false);
}

void generic_windowObj::handlerObj
::do_button_event(ONLY IN_THREAD,
		  const xcb_button_release_event_t *event,
		  const button_event &be,
		  const motion_event &me)
{
	// report_pointer_xy() might not always set
	// most_recent_element_with_pointer(IN_THREAD).

	if (most_recent_element_with_pointer(IN_THREAD))
	{
		elementimpl pointer_element{
			most_recent_element_with_pointer(IN_THREAD)
				};

		// If there's an element with keyboard focus, this pointer
		// click will make it lose. Give it an opportunity to veto
		// this action. This is used by editor_elementObj::implObj
		// to validate the input field's contents.

		if (most_recent_keyboard_focus(IN_THREAD) &&
		    pointer_element->activate_for(be))
		{
			elementimpl keyboard_element{
				&most_recent_keyboard_focus(IN_THREAD)
					->get_focusable_element()
					};

			if (keyboard_element != pointer_element &&
			    !most_recent_keyboard_focus(IN_THREAD)
			    ->ok_to_lose_focus(IN_THREAD, &be))
				return;
		}

		pointer_element
			->unschedule_hover_action(IN_THREAD);

		if (!pointer_element
		    ->process_button_event_if_enabled(IN_THREAD, be,
						      event->time)

		    // Clicking pointer button 1 nowhere in particular removes
		    // keyboard
		    // focus from anything that might have it, right now.
		    && be.button == 1 && be.press)
			unset_keyboard_focus(IN_THREAD, &be);
	}
}

void generic_windowObj::handlerObj::grab(ONLY IN_THREAD,
					 const ref<elementObj::implObj> &e)
{
	set_element_with_pointer(IN_THREAD, e);
	if (most_recent_element_with_pointer(IN_THREAD))
		keep_passive_grab(IN_THREAD);
}

void generic_windowObj::handlerObj::grab(ONLY IN_THREAD)
{
	throw EXCEPTION("Internal error: called grab() on the top level window.");
}

void generic_windowObj::handlerObj::configure_notify_received(ONLY IN_THREAD,
							      const rectangle
							      &r)
{
	mpobj<rectangle>::lock lock(current_position);

	if (*lock == r)
		return;

	if (lock->width != r.width ||
	    lock->height != r.height)
	{
		// Exposure event coming. We use the default bit-gravity of
		// Forget, meaning that we should always get EXPOSURE events
		// after a size change.
		has_exposed(IN_THREAD)=false;

		// And we can throw away all accumulated exposure rectangles
		// because we are guaranteed to get new ones.
		exposure_rectangles(IN_THREAD).rectangles.clear();
	}
	*lock=r;
}

void generic_windowObj::handlerObj::process_configure_notify(ONLY IN_THREAD,
							     const rectangle &r)
{
	returned_pointer<xcb_generic_error_t *> error;

	auto c=conn()->conn;

	auto value=return_pointer(xcb_translate_coordinates_reply
				  (c, xcb_translate_coordinates
				   (c, id(), screenref->impl->xcb_screen->root,
				    0, 0),
				   error.addressof()));

	if (error)
		throw EXCEPTION(connection_error(error));

	auto old_x=root_x(IN_THREAD);
	auto old_y=root_y(IN_THREAD);

	root_x(IN_THREAD)=value->dst_x;
	root_y(IN_THREAD)=value->dst_y;

	root_xy=std::tuple{value->dst_x, value->dst_y};

	// same_screen=value->same_screen;
	// child_window=value->child;

	auto new_position=element_position(r);

	// If the relative position changed, send out the notifications.
	update_current_position(IN_THREAD, new_position);

	// If the absolute location changed, do that too. We can't rely
	// on update_current_position() doing the job. If something wanted
	// to know when its absolute location has changed, it's possible
	// that its parent element's position remains unchanged, so
	// update_current_position() won't trickle down to it.

	if (old_x != root_x(IN_THREAD) || old_y != root_x(IN_THREAD))
		absolute_location_updated(IN_THREAD);
}

void generic_windowObj::handlerObj::current_position_updated(ONLY IN_THREAD)
{
	schedule_update_position_processing(IN_THREAD);
}

void generic_windowObj::handlerObj::horizvert_updated(ONLY IN_THREAD)
{
	if (data(IN_THREAD).metrics_update_callback)
		try {
			auto hv=get_horizvert(IN_THREAD);

			data(IN_THREAD).metrics_update_callback
				(IN_THREAD, hv->horiz, hv->vert);
		} REPORT_EXCEPTIONS(this);
}

bool generic_windowObj::handlerObj::process_key_event(ONLY IN_THREAD,
						      const key_event &ke)
{
	if (prev_key_pressed(ke))
	{
		if (most_recent_keyboard_focus(IN_THREAD))
		{
			if (!most_recent_keyboard_focus(IN_THREAD)
			    ->ok_to_lose_focus(IN_THREAD, &ke))
				return true;
			most_recent_keyboard_focus(IN_THREAD)
				->prev_focus(IN_THREAD, prev_key{});
			return true;
		}

		for (auto b=focusable_fields(IN_THREAD).begin(),
			     e=focusable_fields(IN_THREAD).end();
		     b != e;)
		{
			--e;
			const auto &element=*e;

			if (element->focusable_enabled(IN_THREAD))
			{
				element->set_focus_and_ensure_visibility
					(IN_THREAD, prev_key{});
				return true;
			}
		}
	}

	if (next_key_pressed(ke))
	{
		if (most_recent_keyboard_focus(IN_THREAD))
		{
			if (!most_recent_keyboard_focus(IN_THREAD)
			    ->ok_to_lose_focus(IN_THREAD, &ke))
				return true;

			most_recent_keyboard_focus(IN_THREAD)
				->next_focus(IN_THREAD, next_key{});
			return true;
		}
		return set_default_focus(IN_THREAD, next_key{});
	}
	return false;
}

bool generic_windowObj::handlerObj
::set_default_focus(ONLY IN_THREAD,
		    const callback_trigger_t &trigger)
{
	if (most_recent_keyboard_focus(IN_THREAD))
		return true;

	for (const auto &element:focusable_fields(IN_THREAD))
	{
		if (element->focusable_enabled(IN_THREAD))
		{
			element->set_focus_and_ensure_visibility(IN_THREAD,
								 trigger);
			return true;
		}
	}
	return false;
}

void generic_windowObj::handlerObj
::focusable_initialized(ONLY IN_THREAD, focusableImplObj &fimpl)
{
}

void generic_windowObj::handlerObj::get_focus_first(ONLY IN_THREAD,
						    const focusable &f)
{
	auto b=focusable_fields(IN_THREAD).begin();

	if (b == focusable_fields(IN_THREAD).end())
		throw EXCEPTION("Internal error, there should be at least one focusable field.");

	// Take the first focusable in focusable_fields.
	//
	// Starting with the last focusable in f's focusable group,
	// and ending with the first one:
	//
	// It gets focus before the first focusable_field, then we iterate,
	// setting "it" to the focusable field we just moved up front.

	f->get_impl([&]
		    (const auto &f_group)
		    {
			    auto current_first=*b;

			    size_t n=f_group.internal_impl_count;

			    while (n)
			    {
				    auto i=f_group.impls[--n];

				    i->get_focus_before(IN_THREAD,
							current_first);
				    current_first=i;
			    }
		    });
}

void generic_windowObj::handlerObj
::unset_keyboard_focus(ONLY IN_THREAD,
		       const callback_trigger_t &trigger)
{
	if (most_recent_keyboard_focus(IN_THREAD))
	{
		focusable_impl f=most_recent_keyboard_focus(IN_THREAD);

		most_recent_keyboard_focus(IN_THREAD)=nullptr;

		f->get_focusable_element()
			.lose_focus(IN_THREAD,
				    &elementObj::implObj
				    ::report_keyboard_focus, trigger);
	}

	// Notify the XIM server that we do not have input focus.

	with_xim_client([&]
			(const auto &client)
			{
				client->focus_state(IN_THREAD, false);
			});
}

void generic_windowObj::handlerObj
::set_keyboard_focus_to(ONLY IN_THREAD, const focusable_impl &element,
			const callback_trigger_t &trigger)
{
	auto old_focus=most_recent_keyboard_focus(IN_THREAD);

	most_recent_keyboard_focus(IN_THREAD)=element;

	auto &e=element->get_focusable_element();

	e.request_focus(IN_THREAD, old_focus,
			&elementObj::implObj::report_keyboard_focus,
			trigger);

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
::pointer_motion_event(ONLY IN_THREAD,
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
::enter_notify_event(ONLY IN_THREAD,
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
::leave_notify_event(ONLY IN_THREAD,
		     const xcb_leave_notify_event_t *event)
{
	pointer_focus_lost(IN_THREAD);
}

void generic_windowObj::handlerObj
::focus_change_event(ONLY IN_THREAD, bool flag)
{
	has_focus(IN_THREAD)=flag;
	if (most_recent_keyboard_focus(IN_THREAD))
		most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
			.window_focus_change(IN_THREAD, flag);
}

ref<generic_windowObj::handlerObj> generic_windowObj::handlerObj
::report_pointer_xy(ONLY IN_THREAD,
		    motion_event &me)
{
	return report_pointer_xy(IN_THREAD, me, grab_locked(IN_THREAD));
}

ptr<generic_windowObj::handlerObj>
generic_windowObj::handlerObj::get_popup_parent(ONLY IN_THREAD)
{
	return ptr<generic_windowObj::handlerObj>(this);
}

ref<generic_windowObj::handlerObj> generic_windowObj::handlerObj
::report_pointer_xy(ONLY IN_THREAD,
		    motion_event &me,
		    bool was_grabbed)
{
	// We also need to check for active grabs, which take precedence:

	auto pg=current_pointer_grab(IN_THREAD).getptr();

	if (pg)
	{
		// Did an element grab the pointer?

		auto grabbing_element=pg->get_grab_element(IN_THREAD);

		if (grabbing_element)
		{
			// Ok, make sure that motion event coordinates
			// get translated to the element's actual top level
			// window or popup.
			ref<handlerObj>
				grabbed_element_window{&grabbing_element
					->get_window_handler()};

			add_root_xy(IN_THREAD, me.x, me.y);
			grabbed_element_window
				->subtract_root_xy(IN_THREAD, me.x, me.y);

			if (grabbed_element_window != ref<handlerObj>(this))
				was_grabbed=grabbed_element_window
					->grab_locked(IN_THREAD);

			grabbed_element_window
				->report_pointer_xy_to_this_handler
				(IN_THREAD, pg, me, was_grabbed);
			return grabbed_element_window;
		}

		auto me2=me;

		add_root_xy(IN_THREAD, me2.x, me2.y);
		auto popup=most_recent_popup_with_pointer(IN_THREAD).getptr();
		auto new_popup=handler_data->find_popup_for_xy(IN_THREAD, me2);

		// If we previously reported a motion event to a popup see if
		// we can report the new motion event to the same popup.

		if (popup)
		{
			popup->subtract_root_xy(IN_THREAD, me2.x, me2.y);

			if (new_popup && new_popup != popup)
			{
				// No, a new popup superceded it. We still
				// need to simulate reporting an out of bounds
				// motion event to that popup.

				me2.x= -1;
				me2.y= -1;
			}

			bool remained_inside=me2.x >= 0 && me2.y >= 0
				&& dim_t::truncate(me2.x) < popup->get_width()
				&& dim_t::truncate(me2.y) < popup->get_height();

			popup->report_pointer_xy_to_this_handler
				(IN_THREAD,
				 pg,
				 me2,
				 was_grabbed);

			if (remained_inside)
				return popup; // This is where we reported this.
		}

		if (new_popup)
		{
			add_root_xy(IN_THREAD, me.x, me.y);
			new_popup->subtract_root_xy(IN_THREAD, me.x, me.y);
			most_recent_popup_with_pointer(IN_THREAD).getptr()
				=new_popup;
			new_popup->report_pointer_xy_to_this_handler
				(IN_THREAD, pg, me, was_grabbed);
			return new_popup;
		}
		// Clicking outside of all menus closes them.
		else if (me.type == motion_event_type::button_action_event)
			handler_data->close_all_menu_popups(IN_THREAD);
	}

	report_pointer_xy_to_this_handler(IN_THREAD, pg,
					  me, was_grabbed);
	return ref<handlerObj>(this);
}

void generic_windowObj::handlerObj
::report_pointer_xy_to_this_handler(ONLY IN_THREAD,
				    const grabbed_pointerptr &pg,
				    motion_event me,
				    bool was_grabbed)
{
	auto g=most_recent_element_with_pointer(IN_THREAD);

	if (is_input_busy())
	{
		pointer_focus_lost(IN_THREAD);
		return;
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
::set_element_with_pointer(ONLY IN_THREAD, const ref<elementObj::implObj> &e)
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
		update_displayed_cursor_pointer(IN_THREAD);

		e->request_focus(IN_THREAD, old,
				 &elementObj::implObj::report_pointer_focus,
				 {});
	}
}

void generic_windowObj::handlerObj
::removing_element_from_window(ONLY IN_THREAD,
			       const ref<elementObj::implObj> &ei)
{
	if (most_recent_element_with_pointer(IN_THREAD) == ei)
	{
		ungrab(IN_THREAD);
		pointer_focus_lost(IN_THREAD);
	}
}

void generic_windowObj::handlerObj::pointer_focus_lost(ONLY IN_THREAD)
{
	if (grab_locked(IN_THREAD))
		return;

	auto cpy=most_recent_element_with_pointer(IN_THREAD);

	if (cpy.null())
		return;

	most_recent_element_with_pointer(IN_THREAD)=nullptr;
	update_displayed_cursor_pointer(IN_THREAD);
	cpy->lose_focus(IN_THREAD,
			&elementObj::implObj::report_pointer_focus,
			{});
}

void generic_windowObj::handlerObj::update_frame_extents(ONLY IN_THREAD)
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

void generic_windowObj::handlerObj::frame_extents_updated(ONLY IN_THREAD)
{
}

void generic_windowObj::handlerObj::set_window_type(const std::string &s)
{
	thread()->run_as
		([s,
		  connection_impl=screenref->get_connection()->impl,
		  me=ref<generic_windowObj::handlerObj>(this)]
		 (ONLY IN_THREAD)
		 {
			 mpobj<ewmh>::lock lock(connection_impl->ewmh_info);

			 lock->set_window_type(me->id(), s);
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
::set_window_title(const std::string_view &s)
{
	thread()->run_as
		([title=std::string{s},
		  connection_impl=screenref->get_connection()->impl,
		  me=ref<generic_windowObj::handlerObj>(this)]
		 (ONLY IN_THREAD)
		 {
			 mpobj<ewmh>::lock lock(connection_impl->ewmh_info);

			 lock->set_window_name(me->id(), title);
		 });
}

void generic_windowObj::handlerObj::paste(ONLY IN_THREAD, xcb_atom_t clipboard,
					  xcb_timestamp_t timestamp)
{
	incremental_conversion_in_progress=false;

	convert_selection(IN_THREAD, clipboard,
			  IN_THREAD->info->atoms_info.utf8_string,
			  timestamp);
}

void generic_windowObj::handlerObj
::conversion_failed(ONLY IN_THREAD, xcb_atom_t clipboard,
		    xcb_atom_t type,
		    xcb_timestamp_t timestamp)
{
	if (type == IN_THREAD->info->atoms_info.utf8_string)
		convert_selection(IN_THREAD, clipboard,
				  IN_THREAD->info->atoms_info.string,
				  timestamp);
}

bool generic_windowObj::handlerObj
::begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
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
::converting_incrementally(ONLY IN_THREAD,
			   xcb_atom_t type,
			   xcb_timestamp_t timestamp,
			   uint32_t estimated_size)
{
	incremental_conversion_in_progress=true;
	received_converted_data=true;
}

void generic_windowObj::handlerObj
::converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
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
::end_converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
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
::pasted_string(ONLY IN_THREAD, const std::u32string_view &s)
{
	if (most_recent_keyboard_focus(IN_THREAD))
		most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
			.pasted(IN_THREAD, s);
}

void generic_windowObj::handlerObj::set_input_focus(ONLY IN_THREAD)
{
	xcb_set_input_focus(conn()->conn, XCB_NONE, id(),
			    IN_THREAD->timestamp(IN_THREAD));
}


void generic_windowObj::handlerObj
::update_displayed_cursor_pointer(ONLY IN_THREAD)
{
	cursor_pointerptr pointer_that_should_be_displayed;

	if (most_recent_element_with_pointer(IN_THREAD))
	{
		pointer_that_should_be_displayed=
			most_recent_element_with_pointer(IN_THREAD)
			->get_cursor_pointer(IN_THREAD);
	}

	if (is_wait_busy())
		pointer_that_should_be_displayed=
			tagged_cursor_pointer(IN_THREAD);

	xcb_cursor_t xcb_cursor_t_that_should_be_displayed=XCB_NONE;
	if (pointer_that_should_be_displayed)
		xcb_cursor_t_that_should_be_displayed=
			pointer_that_should_be_displayed->cursor_id();

	xcb_cursor_t xcb_cursor_t_being_displayed=XCB_NONE;
	if (displayed_cursor_pointer)
		xcb_cursor_t_being_displayed=
			displayed_cursor_pointer->cursor_id();

	if (xcb_cursor_t_that_should_be_displayed ==
	    xcb_cursor_t_being_displayed)
		return;

	displayed_cursor_pointer=pointer_that_should_be_displayed;

	values_and_mask change_notify{XCB_CW_CURSOR,
			xcb_cursor_t_that_should_be_displayed};

	xcb_change_window_attributes(IN_THREAD->info->conn,
				     id(),
				     change_notify.mask(),
				     change_notify.values().data());
}

LIBCXXW_NAMESPACE_END
