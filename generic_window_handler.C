/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "shared_handler_data.H"
#include "connection_thread.H"
#include "connectionfwd.H"
#include "defaulttheme.H"
#include "pictformat.H"
#include "pixmap_extractor.H"
#include "x/w/impl/icon.H"
#include "icon_images_set_element.H"
#include "busy.H"
#include "cursor_pointer_element.H"
#include "x/w/impl/draw_info.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/layoutmanager.H"
#include "screen.H"
#include "screen_depthinfo.H"
#include "xid_t.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/impl/popup/popup.H"
#include "cursor_pointer.H"
#include "selection/current_selection_paste_handler.H"
#include "focus/label_for.H"
#include "x/w/impl/focus/focusable.H"
#include "grabbed_pointer.H"
#include "x/w/impl/focus/delayed_input_focus.H"
#include "xim/ximclient.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/values_and_mask.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/main_window.H"
#include "x/w/generic_window_appearance.H"
#include "x/w/impl/child_element.H"
#include "inherited_visibility_info.H"
#include "hotspot.H"
#include "shortcut/installed_shortcut.H"
#include "catch_exceptions.H"
#include <x/property_value.H>
#include <x/weakcapture.H>
#include <x/pidinfo.H>
#include <courier-unicode.h>
#include <xcb/xcb_icccm.h>
#include <string>
#include <algorithm>
LIBCXXW_NAMESPACE_START

static property::value<bool> disable_grabs(LIBCXX_NAMESPACE_STR
					   "::w::disable_grab", false);

static property::value<unsigned> double_click(LIBCXX_NAMESPACE_STR
					      "::w::double_click", 1000);

static property::value<unsigned> resize_timeout(LIBCXX_NAMESPACE_STR
						"::w::resize_timeout", 1000);

static rectangle element_position(const rectangle &r)
{
	auto cpy=r;

	cpy.x=0;
	cpy.y=0;
	return cpy;
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

// Modern technology meets ancient protocols.
//
// We need to set the background-pixel, and all we have is a RENDER-based
// picture, representing the window's background color.
//
// Create a one pixel pixmap, and render the background color into its
// picture, then pluck out the brave pixel.
//
// Fortunately, we will do it only when needed, when constructing the new
// window, using its initial background color, and when the background color
// changes.

static auto compute_background_pixel(ONLY IN_THREAD,
				     const screen &parent_screen,
				     const background_color &col,
				     const rectangle &window_rectangle)
{
	auto pf=parent_screen->impl->toplevelwindow_pictformat;

	auto pm=parent_screen->create_pixmap(pf, 1, 1);

	auto pic=pm->create_picture();

	pic->composite(col->get_current_color(IN_THREAD),
		       coord_t::truncate(window_rectangle.width)-1,
		       coord_t::truncate(window_rectangle.height)-1,
		       0, 0, 1, 1);

	pixmap_extractor pex{pm};

	return pex.get_pixel(0, 0);
}

//////////////////////////////////////////////////////////////////////////

static inline generic_windowObj::handlerObj::extra_constructor_params
create_extra_constructor_params(const generic_window_handler_constructor_params
				&params)
{
	rectangle dimensions={0, 0, 0, 0};

	const auto &parent_screen=params.parent_screen;

	const auto &pf=parent_screen->impl->toplevelwindow_pictformat;

	auto background_color_obj=
		create_new_background_color(parent_screen,
					    pf,
					    params.background_color);

	values_and_mask vm
		{
		 XCB_CW_EVENT_MASK,
		 (uint32_t)
		 generic_windowObj::handlerObj::initial_event_mask(),
		 XCB_CW_COLORMAP,
		 parent_screen->impl->toplevelwindow_colormap->id(),
		 XCB_CW_BIT_GRAVITY,
		 XCB_GRAVITY_NORTH_WEST,
		 XCB_CW_BORDER_PIXEL,
		 parent_screen->impl->xcb_screen->black_pixel,
		 XCB_CW_BACK_PIXEL,
		 compute_background_pixel(parent_screen->impl->thread,
					  parent_screen,
					  background_color_obj,
					  dimensions)
		};

	if (params.override_redirect)
		vm.m[XCB_CW_OVERRIDE_REDIRECT]=1;

	return {
		{
		 parent_screen,
		 parent_screen->impl->xcb_screen->root, // parent
		 pf->depth, // depth
		 dimensions, // initial_position
		 XCB_WINDOW_CLASS_INPUT_OUTPUT, // window_class
		 parent_screen->impl->toplevelwindow_visual
		 ->impl->visual_id, // visual
		 vm, // events_and_mask
		},
		pf,
		params.nesting_level,
		params.background_color,
		background_color_obj,
		params.appearance,
	};
}

generic_windowObj::handlerObj
::handlerObj(const generic_window_handler_constructor_params &params)
	: handlerObj{// Constructor is single-threaded, so we pick IN_THREAD
		     // from here.
		     params.parent_screen->impl->thread,
		     params.handler_data,
		     create_extra_constructor_params(params)}
{
	set_window_type_in_constructor(params.window_type);
	set_window_state_in_constructor(params.window_state);
}

generic_windowObj::handlerObj
::handlerObj(ONLY IN_THREAD,
	     const shared_handler_data &handler_data,
	     const extra_constructor_params &params)
	// This sets up the xcb_window_t
	: window_handlerObj
	{
	 IN_THREAD, params.window_handler_params,
	},
	// And we inherit it as the xcb_drawable_t
	  drawableObj::implObj{
			       IN_THREAD, window_handlerObj::id(),
			       params.drawable_pictformat,
	  },

	  // We can now construct a picture for the window
	  pictureObj::implObj::fromDrawableObj
	{
	 IN_THREAD,window_handlerObj::id(),
	 params.drawable_pictformat->impl->id
	},
	  // We can now construct our graphic context.
	  gcObj::handlerObj{static_cast<drawableObj::implObj &>(*this)},
	// And now, the element that represents the window itself, and
	// all the theme-based resources: background colors, icons, and
	// masks
	  superclass_t
	{
	 params.background_color_obj,
	 create_new_background_color(params.window_handler_params.screenref,
				     params.drawable_pictformat,
				     params.appearance->modal_shade_color),
	 create_new_icon(params.window_handler_params.screenref,
			 params.drawable_pictformat,
			 image_color{params.appearance->disabled_mask}),
	 cursor_pointer::create
	 (create_new_icon(params.window_handler_params.screenref,
			  params.drawable_pictformat,
			  image_color{params.appearance->wait_cursor,
					  render_repeat::none})),
	 params.nesting_level,
	 *this,
	 element_position(params.window_handler_params.initial_position),
	 popupptr{},
	 metrics::horizvert_axi{{0,0,0},{0,0,0}},
	 params.window_handler_params.screenref,
	 params.drawable_pictformat,
	 "background@libcxx.com"
	},
	  current_events_thread_only{(xcb_event_mask_t)
				     params.window_handler_params
				     .events_and_mask.m.at(XCB_CW_EVENT_MASK)},
	  current_position{params.window_handler_params.initial_position},
	  window_pixmap_thread_only
	{params.window_handler_params.screenref
	 ->create_pixmap(params.drawable_pictformat, 0, 0)},
	  window_picture_thread_only{window_pixmap_thread_only
				     ->create_picture()},
	  handler_data{handler_data},
	  my_popups{my_popups_t::create()},
	  original_background_color{params.background_color_arg},
	  frame_extents_thread_only{params.window_handler_params.screenref
				    ->get_workarea()},
	  current_theme_thread_only{params.window_handler_params.screenref
				    ->impl->current_theme.get()},
	  appearance{params.appearance}
{
	top_level_always_visible();

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
	auto conn=screenref->get_connection();

	{
		mpobj<ewmh>::lock lock{conn->impl->ewmh_info};

		lock->request_frame_extents(screenref->impl->screen_number,
					    id());
	}

	update_wm_hints(IN_THREAD);

	// Default graphic context configuration.
	gcObj::handlerObj::configure_gc(ref{this},
					gc::base::properties{});

	// Drag and drop version 5

	xcb_atom_t version=5;
	change_property(IN_THREAD,
			XCB_PROP_MODE_REPLACE,
			conn->impl->info->atoms_info.XdndAware,
			XCB_ATOM_ATOM,
			sizeof(xcb_atom_t)*8,
			1,
			&version);
	// The top level window is not a child element in a container,
	// so it is, hereby, initialized!

	initialize_if_needed(IN_THREAD);
}

void generic_windowObj::handlerObj::update_wm_hints(ONLY IN_THREAD)
{
	update_wm_hints([&, this]
			(xcb_icccm_wm_hints_t &hints)
			{
				this->set_default_wm_hints(IN_THREAD, hints);
			});
}

void generic_windowObj::handlerObj
::client_message_event(ONLY IN_THREAD,
		       const xcb_client_message_event_t *event)
{
	// Drag and drop target processing.

	if (event->type == IN_THREAD->info->atoms_info.XdndEnter)
	{
		process_drag_enter(IN_THREAD, event);
		return;
	}
	if (event->type == IN_THREAD->info->atoms_info.XdndPosition)
	{
		process_drag_position(IN_THREAD, event);
		return;
	}
	if (event->type == IN_THREAD->info->atoms_info.XdndLeave)
	{
		process_drag_leave(IN_THREAD, event);
		return;
	}

	if (event->type == IN_THREAD->info->atoms_info.XdndDrop)
	{
		process_drag_drop(IN_THREAD, event);
		return;
	}

	// Drag and drop source processing.

	if (event->type == IN_THREAD->info->atoms_info.XdndStatus ||
	    event->type == IN_THREAD->info->atoms_info.XdndFinished)
	{
		process_drag_response(IN_THREAD, event);
	}
	window_handlerObj::client_message_event(IN_THREAD, event);
}

////////////////////////////////////////////////////////////////////
//
// Inherited from elementObj::implObj

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
			->get_current_color(IN_THREAD),
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

void generic_windowObj::handlerObj::background_color_changed(ONLY IN_THREAD)
{
	superclass_t::background_color_changed(IN_THREAD);

	update_background_pixel(IN_THREAD,
				current_background_color(IN_THREAD));
}

void generic_windowObj::handlerObj
::process_updated_position(ONLY IN_THREAD)
{
	// If we have the misfortune of dealing with a gradient color,
	// we may need to update our background-pixel whenever the
	// window's size changes.
	//
	// We detect it by saving the current_background_color before
	// processing the position update, and see if it changed after
	// processing it.
	auto old_background_color=current_background_color(IN_THREAD);
	superclass_t::process_updated_position(IN_THREAD);

	auto new_background_color=current_background_color(IN_THREAD);

	if (old_background_color == new_background_color)
		return;

	// Gradient color changed.

	update_background_pixel(IN_THREAD, new_background_color);
}

void generic_windowObj::handlerObj
::update_background_pixel(ONLY IN_THREAD,
			  const background_color &col)
{
	values_and_mask vm
		{
		 XCB_CW_BACK_PIXEL,
		 compute_background_pixel(IN_THREAD, get_screen(),
					  col,
					  data(IN_THREAD).current_position),
		};

	xcb_change_window_attributes(IN_THREAD->info->conn,
				     id(),
				     vm.mask(),
				     vm.values().data());
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

rectangle generic_windowObj::handlerObj::get_absolute_location(ONLY IN_THREAD)
	const
{
	return data(IN_THREAD).current_position;
}

void generic_windowObj::handlerObj
::get_absolute_location_on_screen(ONLY IN_THREAD, rectangle &r) const
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

void generic_windowObj::handlerObj::update_visibility(ONLY IN_THREAD)
{
	// Ignore visibility updates until such time we are
	// initialize_if_needed(). Same check as the overridden
	// elementObj::implObj

	if (!initialized(IN_THREAD))
		return;

	visibility_updated(IN_THREAD,
			   data(IN_THREAD).requested_visibility);
}

void generic_windowObj::handlerObj
::inherited_visibility_updated(ONLY IN_THREAD,
			       inherited_visibility_info &visibility_info)
{
	visibility_info.do_not_redraw=true;

	do_inherited_visibility_updated(IN_THREAD, visibility_info);

	// Ok, so if we're now visible, find the first widget that can
	// autofocus() and give it keyboard focus.

	if (visibility_info.flag)
	{
		// But if there's a widget that requested a delayed keyboard
		// focus, and we can give it to it, this will be good enough.

		if (process_focus_updates(IN_THREAD))
			return;

		for (const auto &element:focusable_fields(IN_THREAD))
		{
			if (!element->focusable_enabled(IN_THREAD))
				continue;

			if (!element->autofocus.get())
				continue;

			element->set_focus_and_ensure_visibility
				(IN_THREAD, keyfocus_move{});
			break;
		}
	}
}

void generic_windowObj::handlerObj
::set_inherited_visibility_flag(ONLY IN_THREAD,
				bool logical_flag,
				bool reported_flag)
{
	superclass_t::set_inherited_visibility_flag(IN_THREAD,
						    true,
						    reported_flag);

	if (reported_flag)
	{
		set_inherited_visibility_mapped(IN_THREAD);
	}
	else
	{
		set_inherited_visibility_unmapped(IN_THREAD);
	}
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
			     create_new_background_color
			     (screenref,
			      drawable_pictformat,
			      original_background_color));
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
	update_window_pixmap_and_picture(IN_THREAD,
					 data(IN_THREAD).current_position);

	exposure_event_recursive
		(IN_THREAD,
		 exposure_rectangles(IN_THREAD).rectangles,
		 exposure_type::actual_exposure);
	invoke_stabilized(IN_THREAD);
}

void generic_windowObj::handlerObj
::process_collected_graphics_exposures(ONLY IN_THREAD)
{
	exposure_event_recursive
		(IN_THREAD,
		 graphics_exposure_rectangles(IN_THREAD).rectangles,
		 exposure_type::actual_exposure);
}

void generic_windowObj::handlerObj::theme_updated_event(ONLY IN_THREAD)
{
	auto new_theme=get_screen()->impl->current_theme.get();

	if (new_theme == current_theme(IN_THREAD))
		return;
	current_theme(IN_THREAD)=new_theme;

	theme_updated(IN_THREAD, new_theme);
}

void generic_windowObj::handlerObj::theme_updated(ONLY IN_THREAD,
						  const const_defaulttheme
						  &new_theme)
{
	auto b=current_background_color(IN_THREAD);

	superclass_t::theme_updated(IN_THREAD, new_theme);

	if (b == current_background_color(IN_THREAD))
		return;

	// Background color changed (2/2).
	background_color_changed(IN_THREAD);
}

// Shade mcguffin.

// The destructor schedules a redraw of the entire window, when the
// shade mcguffin gets destroyed.

namespace {
#if 0
}
#endif

class busy_shadeObj : virtual public obj {

 public:
	//! My window
	const weakptr<ptr<generic_windowObj::handlerObj>> handler;

	//! The mcguffin that gets inserted into all_shade_mcguffins
	const ref<obj> mcguffin;

	//! The mcguffin's iterator in the handler's all_shade_mcguffins.

	//! This iterator is initialized while holding a lock on the
	//! busy_mcguffins, and gets sequenced accordingly.

	std::list<ref<obj>>::iterator iterator;

	//! Constructor
	busy_shadeObj(const ref<generic_windowObj::handlerObj> &handler)
		: handler{handler}, mcguffin{ref<obj>::create()}
	{
	}

	//! Destructor
	~busy_shadeObj()
	{
		auto p=handler.getptr();

		if (!p)
			return;

		// Remove the shade in the connection thread.
		p->thread()->run_as
			([me=ref{&*p}, iterator=this->iterator]
			 (ONLY IN_THREAD)
			 {
				 me->shade_mcguffin_destroyed(IN_THREAD,
							      iterator);
			 });
	}
};

// wait mcguffin

// The destructor calls update_displayed_cursor_pointer.

class busy_waitObj : virtual public obj {

 public:
	const weakptr<ptr<generic_windowObj::handlerObj>> handler;

	busy_waitObj(const ref<generic_windowObj::handlerObj> &handler)
		: handler{handler}
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

		auto h=ref{&*p};

		h->thread()->run_as
			([h]
			 (ONLY IN_THREAD)
			 {
				 h->update_displayed_cursor_pointer(IN_THREAD);
			 });
	}
};

#if 0
{
#endif
}

ref<obj> generic_windowObj::handlerObj::get_shade_busy_mcguffin()
{
	if (drawable_pictformat->alpha_depth == 0)
		return get_wait_busy_mcguffin();

	busy_mcguffins_t::lock lock{busy_mcguffins};

	// If there's a main mcguffin object, return it.
	auto p=lock->shade.getptr();

	if (p) return p;

	auto n=ref<busy_shadeObj>::create(ref{this});

	lock->shade=n;

	// Do the rest of the work in the connection thread
	//
	// The lambda captures n, so the new shade mcguffin is guaranteed
	// to remain in scope until the lambda gets executed.
	thread()->run_as
		([n, me=ref{this}]
		 (ONLY IN_THREAD)
		 {
			 busy_mcguffins_t::lock lock{me->busy_mcguffins};

			 // Is this the first shade mcguffin?

			 bool was_empty=lock->all_shade_mcguffins.empty();

			 lock->all_shade_mcguffins.push_front(n->mcguffin);

			 n->iterator=lock->all_shade_mcguffins.begin();

			 if (!was_empty)
				 return;

			 // Instead of redrawing each element, we'll just
			 // compose the shade directly into the window_pixmap,
			 // then flush_redrawn_areas.

			 rectangle r{
				     0, 0,
				     me->window_pixmap(IN_THREAD)->get_width(),
				     me->window_pixmap(IN_THREAD)->get_height()
			 };

			 me->window_picture(IN_THREAD)->composite
				 (me->shaded_color(IN_THREAD)
				  ->get_current_color(IN_THREAD),
				  0, 0, r,
				  render_pict_op::op_atop);

			 rectarea rr{r};

			 me->flush_redrawn_areas(IN_THREAD, rr);
		 });

	return n;
}

void generic_windowObj::handlerObj
::shade_mcguffin_destroyed(ONLY IN_THREAD,
			   std::list<ref<obj>>::iterator iterator)
{
	// The shade mcguffin has been destroyed. It is possible for race
	// conditions to cause more than one shade mcguffin to exist at the
	// same time.
	//
	// This is why shade mcguffins' construction inserts each new shade
	// mcguffin's into all_shade_mcguffins. Here, one shade mcguffin
	// has been destroyed. If the list of all_shade_mcguffins is now
	// empty, this must be the last one of them.
	//
	// get_shade_busy_mcguffin() composes the shade into the window
	// when the first shade mcguffin is created, and now we will
	// unshade by redrawing everything.

	{
		busy_mcguffins_t::lock lock{busy_mcguffins};

		lock->all_shade_mcguffins.erase(iterator);

		if (!lock->all_shade_mcguffins.empty())
			return;
	}

	explicit_redraw_recursively(IN_THREAD);
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
	// We consider input to be blocked if either the wait cursor
	// or the wait_cursor exists, or if all_shade_mcguffins exist.
	//
	// We want input blocked as soon as someone creates a shade
	// mcguffin, except that all_shade_mcguffins doesn't get a new
	// mcguffin until that process concludes in the connection thread.
	//
	// So we check both: shade_mcguffin and all_shade_mcguffins.

	busy_mcguffins_t::lock lock{busy_mcguffins};

	return !!lock->shade.getptr()
		|| !lock->all_shade_mcguffins.empty()
		|| !!lock->wait_cursor.getptr();
}

bool generic_windowObj::handlerObj::is_shade_busy()
{
	// This is called from drawn_to_window_picture() to determine whether
	// a shade needs to be composed on top of the drawn content. Whether
	// a shade is drawn is controlled by non-empty all_shade_mcguffins
	// list, so this is what we check here.

	busy_mcguffins_t::lock lock{busy_mcguffins};

	return !lock->all_shade_mcguffins.empty();
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
	// Popup has grabbed pointer and keyboard input?

	auto pg=current_pointer_grab(IN_THREAD).getptr();

	if (pg)
	{
		if (handler_data->handle_key_event(IN_THREAD, ref(this),
						   event, sequencehi,
						   keypress))
			return;
	}

	forward_key_event_to_xim(IN_THREAD, event, sequencehi, keypress);
}

void generic_windowObj::handlerObj
::forward_key_event_to_xim(ONLY IN_THREAD,
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

	return handle_key_event(IN_THREAD, ke);
}

bool generic_windowObj::handlerObj::handle_key_event(ONLY IN_THREAD,
						     const key_event &ke)
{
#ifdef DEBUG_KEY_EVENT
	DEBUG_KEY_EVENT();
#endif
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

	if (!activate_for(ke))
		return false;

	installed_shortcutptr best_shortcut=lookup_shortcut(IN_THREAD, ke);

	if (!best_shortcut)
	{
		auto std_keysym=shortcut::shortcut_keysym(ke.keysym);

		if (std_keysym != ke.keysym)
		{
			auto copy_ke=ke;

			copy_ke.keysym=std_keysym;
			best_shortcut=lookup_shortcut(IN_THREAD, ke);
		}
	}

	if (!best_shortcut)
		return false;

	try {
		best_shortcut->activated(IN_THREAD, &ke);
	} REPORT_EXCEPTIONS(this);
	return true;
}

installed_shortcutptr
generic_windowObj::handlerObj::lookup_shortcut(ONLY IN_THREAD,
					       const key_event &ke)
{
	installed_shortcutptr best_shortcut;
	int best_ordinal=0;

	// Search this window's local shortcuts, and global shortcuts.
	mpobj<shortcut_lookup_t> * const all_shortcuts[]={
		&local_shortcuts,
		&handler_data->global_shortcuts};

	for (const auto shortcut_collection:all_shortcuts)
	{
		mpobj<shortcut_lookup_t>::lock lock{*shortcut_collection};

		auto shortcuts=lock->equal_range(unicode_lc(ke.unicode));

		while (shortcuts.first != shortcuts.second)
		{
			auto p=shortcuts.first->second;

			++shortcuts.first;

			// Find the first shortcut that's enabled and
			// matches the key.

			if (!p->enabled(IN_THREAD)
			    ||
			    !p->installed_shortcut(IN_THREAD).matches(ke))
				continue;

			// If there are shortcuts for both shift-Foo and
			// shift-ctrl-Foo, make sure that
			// shift-ctrl-Foo matches the right shortcut.

			int ordinal=p->installed_shortcut(IN_THREAD).ordinal();

			if (best_shortcut && ordinal == best_ordinal)
			{
				auto &a=p->installed_shortcut(IN_THREAD);

				auto &b=best_shortcut
					->installed_shortcut(IN_THREAD);

				auto s1=unicode::iconvert
					::convert(a.description(),
						  unicode::utf_8,
						  unicode_locale_chset());

				auto s2=unicode::iconvert
					::convert(b.description(),
						  unicode::utf_8,
						  unicode_locale_chset());

				LOG_ERROR("Conflicting shortcuts: "
					  << s1 << " and " << s2);
			}

			if (!best_shortcut || ordinal > best_ordinal)
			{
				best_shortcut=p;
				best_ordinal=ordinal;
			}
		}
	}

	return best_shortcut;
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
#ifdef DEBUG_BUTTON_EVENT
	DEBUG_BUTTON_EVENT();
#endif
	auto &keysyms=
		get_screen()->get_connection()->impl->keysyms_info(IN_THREAD);

	button_event_redirect_info redirect_info;

	button_event be{event->state, keysyms, event->detail, buttonpress,
			click_count, redirect_info};

	motion_event me{be, (activate_for(be)
			     ? motion_event_type::button_action_event
			     : motion_event_type::button_event),
			event->event_x, event->event_y};

	auto report_to=report_pointer_xy(IN_THREAD, me, was_grabbed);

	if (report_to->is_input_busy())
		return;

	report_to->do_button_event(IN_THREAD, event, be, me);

	// The passive grab was released by a button release event.
	// We need to re-report this as a motion event. If the grabbing element
	// had a custom pointer installed and the pointer is no longer in the
	// element, we need to update_displayed_cursor_pointer().

	if (was_grabbed && !buttonpress)
		report_to->report_pointer_xy(IN_THREAD, me, false);
}

element_implptr generic_windowObj::handlerObj
::current_pointer_event_destination(ONLY IN_THREAD)
{
	auto pg=current_pointer_grab(IN_THREAD).getptr();

	if (pg)
	{
		auto grabbing_element=pg->get_grab_element(IN_THREAD);

		if (grabbing_element)
			return grabbing_element;

		auto popup=most_recent_popup_with_pointer(IN_THREAD).getptr();

		if (popup)
		{
			auto g=popup->most_recent_element_with_pointer
				(IN_THREAD);

			if (g)
				return g;
		}
	}

	return most_recent_element_with_pointer(IN_THREAD);
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
		element_impl pointer_element{
			most_recent_element_with_pointer(IN_THREAD)
				};

		// If there's an element with keyboard focus, this pointer
		// click will make it lose. Give it an opportunity to veto
		// this action. This is used by editor_elementObj::implObj
		// to validate the input field's contents.

		if (most_recent_keyboard_focus(IN_THREAD) &&
		    pointer_element->activate_for(be))
		{
			element_impl keyboard_element{
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

void generic_windowObj::handlerObj::process_map_notify_event(ONLY IN_THREAD)
{
	has_mapped(IN_THREAD)=true;

	// This has the effect of sending an InternAtom reques to the server
	// If the server sends exposure events immediately after MapNotify,
	// those are only on the way and we'll get them before the reply
	// to InternAtom. libxcb will effectively buffer up everything it
	// gets until the reply to InternAtom is received, so we'll have
	// all the exposure events collected and we'll process them and then
	// we'll flush_redrawn_areas().
	//
	// It's possible that MapNotify event will cause something to be
	// explicitly drawn ASAP, and this approach will coalesce that
	// together with everything when we flush_redrawn_areas().
	(void)IN_THREAD->info->get_atom("ATOM", true);
}

void generic_windowObj::handlerObj::process_unmap_notify_event(ONLY IN_THREAD)
{
	has_mapped(IN_THREAD)=false;
}

void generic_windowObj::handlerObj::configure_notify_received(ONLY IN_THREAD,
							      const rectangle
							      &r)
{
	do_configure_notify_received(IN_THREAD, r);
}

void generic_windowObj::handlerObj
::do_configure_notify_received(ONLY IN_THREAD,
			       const rectangle &r)
{
	// We can get a bunch of these in a row. Save them, and process them
	// when the dust settles, in process_configure_notify().

	current_position=r;
}

void generic_windowObj::handlerObj::raise(ONLY IN_THREAD)
{
	values_and_mask configure_window_vals
		(XCB_CONFIG_WINDOW_STACK_MODE,
		 XCB_STACK_MODE_ABOVE);

	xcb_configure_window(IN_THREAD->info->conn, id(),
			     configure_window_vals.mask(),
			     configure_window_vals.values().data());

	for (const auto &my_popup:*my_popups)
	{
		auto p=my_popup.getptr();

		if (p)
			p->raise(IN_THREAD);
	}
}

void generic_windowObj::handlerObj::lower(ONLY IN_THREAD)
{
	for (const auto &my_popup:*my_popups)
	{
		auto p=my_popup.getptr();

		if (p)
			p->lower(IN_THREAD);
	}

	values_and_mask configure_window_vals
		(XCB_CONFIG_WINDOW_STACK_MODE,
		 XCB_STACK_MODE_BELOW);

	xcb_configure_window(IN_THREAD->info->conn, id(),
			     configure_window_vals.mask(),
			     configure_window_vals.values().data());
}

void generic_windowObj::handlerObj::process_configure_notify(ONLY IN_THREAD)
{
	do_process_configure_notify(IN_THREAD);
}

void generic_windowObj::handlerObj::do_process_configure_notify(ONLY IN_THREAD)
{
	returned_pointer<xcb_generic_error_t *> error;

	auto c=conn()->conn;

	// Ok, we just received a ConfigureNotify, and exposure are on the
	// way. We're sending a TranslateCoordinates request, and we expect
	// to get the reply to it after we receive all our exposures, if there
	// are any. X protocol specifies that any exposures follow a
	// ConfigureNotify. It's not mandated that they immediately follow it,
	// but we're going to hope that this is the case. So, before we proceed
	// we're going to already have all exposure events received, and waiting
	// to be processed in the main event loop.
	//
	// See process_buffered_events().

	auto value=return_pointer(xcb_translate_coordinates_reply
				  (c, xcb_translate_coordinates
				   (c, id(), screenref->impl->xcb_screen->root,
				    0, 0),
				   error.addressof()));

	auto r=current_position.get();

	if (data(IN_THREAD).current_position.width != r.width ||
	    data(IN_THREAD).current_position.height != r.height)
	{
		if (has_exposed(IN_THREAD))
			update_window_pixmap_and_picture(IN_THREAD, r);
	}
	if (error)
		throw EXCEPTION(connection_error(error));

	if (most_recent_keyboard_focus(IN_THREAD) &&
	    most_recent_keyboard_focus(IN_THREAD)->get_focusable_element()
	    .uses_input_method())
	{
		with_xim_client
			([&]
			 (const auto &client)
			 {
				 client->resend_cursor_position(IN_THREAD);
			 });
	}

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

	update_resizing_timeout(IN_THREAD);

	// If the absolute location changed, do that too. We can't rely
	// on update_current_position() doing the job. If something wanted
	// to know when its absolute location has changed, it's possible
	// that its parent element's position remains unchanged, so
	// update_current_position() won't trickle down to it.

	if (old_x != root_x(IN_THREAD) || old_y != root_y(IN_THREAD))
	{
		absolute_location_updated(IN_THREAD);
		redraw_after_absolute_location_updated
			(IN_THREAD,
			 absolute_location_update_reason::external);
	}
}

void generic_windowObj::handlerObj
::drawing_to_window_picture(ONLY IN_THREAD,
			    const rectangle &rect)
{
	dim_t right=dim_t::truncate(rect.x+rect.width);
	dim_t bottom=dim_t::truncate(rect.y+rect.height);

	auto &current_impl=window_pixmap(IN_THREAD)->impl;

	if (current_impl->width >= right &&
	    current_impl->height >= bottom)
		return;

	elementObj::implObj &me=*this;

	update_window_pixmap_and_picture(IN_THREAD,
					 me.data(IN_THREAD).current_position);
}

void generic_windowObj::handlerObj
::update_window_pixmap_and_picture(ONLY IN_THREAD, const rectangle &r)
{
	auto window_pixmap_width=window_pixmap(IN_THREAD)->get_width();
	auto window_pixmap_height=window_pixmap(IN_THREAD)->get_height();

	auto current_width=window_pixmap_width;
	auto current_height=window_pixmap_height;

#define DEBUG_WINDOW_PIXMAP_RESIZE 0

#if DEBUG_WINDOW_PIXMAP_RESIZE
	std::cout << "Pixmap size should be "
		  << r.width << "x" << r.height << std::endl
		  << "Current pixmap size is "
		  << current_width << "x" << current_height
		  << std::endl
		  << "Current window size is "
		  << data(IN_THREAD).current_position.width
		  << "x"
		  << data(IN_THREAD).current_position.height
		  << std::endl;
#endif

	// If the new window's size is at least as big as the pixmap, we're
	// good, but if the new size is much smaller, we'll go ahead and
	// resize to a smaller pixmap.

	if (current_width >= r.width &&
	    current_height >= r.height &&

	    current_width - r.width < r.width &&
	    current_height - r.height < r.height)
		return;

	// New size is larger than the old size:
	//
	// Make the new window_pixmap even bigger, to accomodate further
	// expansion (we're manually resizing the window), but:
	//
	// If the current size is 0 this must be the initial window size,
	// so size the window_pixmap to the initial window size, otherwise
	// add half the window size, for the extra room.
	//
	// New size is much smaller than the old size:
	//
	// Set the new window_pixmap's size to be halfway between the old
	// size and the new size. This heuristically lets up keep enough
	// of the old contents in the backing store to be able to efficiently
	// scroll widgets, if the shrinkage is due to the widgets moving.

	if (current_width < r.width)
	{
		if (current_width > 0)
			current_width=current_width/2;
		else
			current_width=0;

		current_width=
			dim_t::truncate(current_width+r.width);
	}
	else if (current_width - r.width > r.width)
	{
		current_width=
			dim_t::truncate(r.width + (current_width-r.width)/2);

		if (current_width < data(IN_THREAD).current_position.width)
			current_width=data(IN_THREAD).current_position.width;
	}

	if (current_height < r.height)
	{
		if (current_height > 0)
			current_height=current_height/2;
		else
			current_height=0;

		current_height=
			dim_t::truncate(current_height+r.height);
	}
	else if (current_height - r.height > r.height)
	{
		current_height=
			dim_t::truncate(r.height + (current_height-r.height)/2);
		if (current_height < data(IN_THREAD).current_position.height)
			current_height=data(IN_THREAD).current_position.height;
	}

	auto new_pixmap=create_pixmap(current_width, current_height);

#if DEBUG_WINDOW_PIXMAP_RESIZE
	std::cout << "New pixmap size is "
		  << current_width << "x" << current_height
		  << std::endl << std::endl;
#endif

	// Copy over the contents of the pixmap, for optimized scrolling
	// purposes. That is, if the pixmap is not the initial empty one.

	// But copy the smaller of the new and the old size
	if (window_pixmap_width > current_width)
		window_pixmap_width=current_width;

	if (window_pixmap_height > current_height)
		window_pixmap_height=current_height;

	if (window_pixmap_width > 0 && window_pixmap_height > 0)
	{
		copy_configured({0, 0,
				 window_pixmap_width, window_pixmap_height},
				0, 0,
				window_pixmap(IN_THREAD)->impl,
				new_pixmap->impl);
	}

	window_picture(IN_THREAD)=new_pixmap->create_picture();
	window_pixmap(IN_THREAD)=new_pixmap;
}

void generic_windowObj::handlerObj::scroll_window_pixmap(ONLY IN_THREAD,
							 const rectangle &r,
							 coord_t scrolled_to_x,
							 coord_t scrolled_to_y)
{
	// Effect the scroll using xcb_copy-area

	auto window_drawable=ref{this};

	gcObj::properties props;

	copy(r,
	     scrolled_to_x,
	     scrolled_to_y,
	     window_drawable,
	     window_drawable, props);

	auto &pixmap_impl=window_pixmap(IN_THREAD)->impl;

	copy(r,
	     scrolled_to_x,
	     scrolled_to_y,
	     pixmap_impl,
	     pixmap_impl,
	     props);
}


void generic_windowObj::handlerObj::flush_redrawn_areas(ONLY IN_THREAD)
{
	flush_redrawn_areas(IN_THREAD, window_drawnarea(IN_THREAD));
}

void generic_windowObj::handlerObj::flush_redrawn_areas(ONLY IN_THREAD,
							rectarea &redrawn)
{
	// Wait for the initial exposure.
	if (!has_exposed(IN_THREAD) || !has_mapped(IN_THREAD))
		return;

	// This combines duplicates and merges them.

	auto combined=add(redrawn, redrawn);

	redrawn.clear();

	// If these rectangles are queued up to be redrawn due to exposure,
	// we'll remove them from the list of exposed rectangles.
	//
	// We want to do it this way instead of subtracting exposure and
	// graphics_exposure rectangles from the redrawn rectangles because,
	// for better visual appearance, we want to immediately draw
	// widgets moved by process_container_widget_positions_updated().

	exposure_rectangles(IN_THREAD).rectangles=
		subtract(exposure_rectangles(IN_THREAD).rectangles,
			 combined);

	graphics_exposure_rectangles(IN_THREAD).rectangles=
		subtract(graphics_exposure_rectangles(IN_THREAD).rectangles,
			 combined);

	ref<drawableObj::implObj> me{this};

	for (const auto &r:combined)
	{
#ifdef DEBUG_FLUSH_REDRAWN_AREAS
		DEBUG_FLUSH_REDRAWN_AREAS();
#endif
		copy_configured(r, r.x, r.y,
				window_pixmap(IN_THREAD)->impl, me);
	}
}

void generic_windowObj::handlerObj::current_position_updated(ONLY IN_THREAD)
{
	schedule_update_position_processing(IN_THREAD);
}

void generic_windowObj::handlerObj::horizvert_updated(ONLY IN_THREAD)
{
	update_resizing_timeout(IN_THREAD);

	if (data(IN_THREAD).metrics_update_callback)
		try {
			auto hv=get_horizvert(IN_THREAD);

			data(IN_THREAD).metrics_update_callback
				(IN_THREAD, hv->horiz, hv->vert);
		} REPORT_EXCEPTIONS(this);
}

void generic_windowObj::handlerObj::install_size_hints(const size_hints &hints)
{
}

void generic_windowObj::handlerObj::size_hints_updated(ONLY IN_THREAD)
{
}

void generic_windowObj::handlerObj::update_resizing_timeout(ONLY IN_THREAD)
{
	const auto &pos=data(IN_THREAD).current_position;
	auto hv=get_horizvert(IN_THREAD);

	bool flag=
		data(IN_THREAD).requested_visibility &&
		(
		 (pos.width < hv->horiz.minimum()) ||
		 (pos.width > hv->horiz.maximum()) ||
		 (pos.height < hv->vert.minimum()) ||
		 (pos.height > hv->vert.maximum())
		 );

	if (flag == is_resizing(IN_THREAD))
		return;

	if (!flag)
	{
		resizing(IN_THREAD)=not_resizing{};
	}
	else
	{
		resizing(IN_THREAD)=
			tick_clock_t::now()+
			std::chrono::duration_cast<tick_clock_t::duration>
			(std::chrono::milliseconds(resize_timeout.get()));
	}
	invoke_stabilized(IN_THREAD);
}

void generic_windowObj::handlerObj::invoke_stabilized(ONLY IN_THREAD)
{
	if (is_resizing(IN_THREAD) || !data(IN_THREAD).requested_visibility)
		return;

	auto callbacks=stabilized_callbacks(IN_THREAD);

	stabilized_callbacks(IN_THREAD).clear();

	busy_impl yes_i_am{*this};

	for (const auto &callback:callbacks)
	{
		try {
			callback(IN_THREAD, yes_i_am);
		} REPORT_EXCEPTIONS(this);
	}
}

bool generic_windowObj::handlerObj::process_key_event(ONLY IN_THREAD,
						      const key_event &ke)
{
	if (prev_key_pressed(ke))
	{
		// If there's an element with the keyboard focus,
		// invoke its prev_focus() method, and we're done.

		if (most_recent_keyboard_focus(IN_THREAD))
		{
			if (!most_recent_keyboard_focus(IN_THREAD)
			    ->ok_to_lose_focus(IN_THREAD, &ke))
				return true;
			most_recent_keyboard_focus(IN_THREAD)
				->prev_focus(IN_THREAD, prev_key{});
			return true;
		}

		// No element with keyboard focus, find the last one.

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
		// No elements that can accept keyboard focus, transfer
		// keyboard focus to the next window.

		return transfer_focus_to_next_window(IN_THREAD);
	}

	if (next_key_pressed(ke))
	{
		// If there's an element with the keyboard focus,
		// invoke its next_focus() method, and we're done.

		if (most_recent_keyboard_focus(IN_THREAD))
		{
			if (!most_recent_keyboard_focus(IN_THREAD)
			    ->ok_to_lose_focus(IN_THREAD, &ke))
				return true;

			most_recent_keyboard_focus(IN_THREAD)
				->next_focus(IN_THREAD, next_key{});
			return true;
		}

		// No element with the keyboard focus, find the first one.

		if (set_default_focus(IN_THREAD, next_key{}))
			return true;

		// No elements that can accept keyboard focus, transfer
		// keyboard focus to the next window.

		return transfer_focus_to_next_window(IN_THREAD);
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
::focusable_initialized(ONLY IN_THREAD, focusableObj::implObj &fimpl)
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

focusable_implptr generic_windowObj::handlerObj
::get_autorestorable_focusable()
{
	autorestorable_focusable_t::lock lock{autorestorable_focusable};

	auto p=lock->getptr();

	return p;
}

element_implptr generic_windowObj::handlerObj
::element_that_can_receive_selection()
{
	auto p=get_autorestorable_focusable();

	if (p)
	{
		auto &e=p->get_focusable_element();

		if (e.selection_can_be_received())
			return ptr{&e};
	}

	return {};
}

void generic_windowObj::handlerObj
::set_keyboard_focus_to(ONLY IN_THREAD, const focusable_impl &element,
			const callback_trigger_t &trigger)
{
	// Clear any delayed input focus mcguffin.
	//
	// The fact that we're here must mean that it's getting the input
	// focus, or someone else decided to get the input focus first.

	if (trigger.index() != callback_trigger_keyfocus_move)
		remove_delayed_keyboard_focus(IN_THREAD);

	auto old_focus=most_recent_keyboard_focus(IN_THREAD);

	if (old_focus == element)
		return; // Keyboard focus unchanged

	most_recent_keyboard_focus(IN_THREAD)=element;

	auto &e=element->get_focusable_element();

	e.request_focus(IN_THREAD, old_focus,
			&elementObj::implObj::report_keyboard_focus,
			trigger);

	if (element->focus_autorestorable(IN_THREAD))
		autorestorable_focusable=element;

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
::remove_delayed_keyboard_focus(ONLY IN_THREAD)
{
	auto mcguffin=scheduled_input_focus(IN_THREAD).getptr();

	if (!mcguffin)
		return;

	auto f=mcguffin->me(IN_THREAD).getptr();

	if (!f)
		return;

	f->delayed_input_focus_mcguffin(IN_THREAD)=nullptr;

#ifdef TEST_UNINSTALL_DELAYED_MCGUFFIN
	TEST_UNINSTALL_DELAYED_MCGUFFIN();
#endif
}

void generic_windowObj::handlerObj
::pointer_motion_event(ONLY IN_THREAD,
		       const xcb_motion_notify_event_t *event)
{
#ifdef DEBUG_POINTER_MOTION_EVENT
	DEBUG_POINTER_MOTION_EVENT();
#endif
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
	pointer_focus_lost(IN_THREAD, {});
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

			handler_data->reporting_pointer_xy_to
				(IN_THREAD,
				 ref{this},
				 grabbed_element_window,
				 &me);

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

			if (remained_inside)
			{
				handler_data->reporting_pointer_xy_to
					(IN_THREAD,
					 ref{this},
					 popup,
					 &me);
				popup->report_pointer_xy_to_this_handler
					(IN_THREAD,
					 pg,
					 me2,
					 was_grabbed);

				return popup; // This is where we reported this.
			}
		}

		if (new_popup)
		{
			add_root_xy(IN_THREAD, me.x, me.y);
			new_popup->subtract_root_xy(IN_THREAD, me.x, me.y);
			most_recent_popup_with_pointer(IN_THREAD).getptr()
				=new_popup;
			handler_data->reporting_pointer_xy_to
				(IN_THREAD,
				 ref{this},
				 new_popup,
				 &me);

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
		pointer_focus_lost(IN_THREAD, &me);
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

	element_impl e{find_element_under(IN_THREAD, me.x, me.y)};

	set_element_with_pointer(IN_THREAD, e);

	if (most_recent_element_with_pointer(IN_THREAD))
		most_recent_element_with_pointer(IN_THREAD)
			->report_motion_event(IN_THREAD, me);
}

element_impl generic_windowObj::handlerObj
::find_element_under(ONLY IN_THREAD, coord_t &x, coord_t &y)
{
	element_impl e{this};

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

					  if (!child
					      ->can_be_under_pointer(IN_THREAD))
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

	return e;
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
		pointer_focus_lost(IN_THREAD, {});
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
		pointer_focus_lost(IN_THREAD, {});
	}
}

void generic_windowObj::handlerObj::pointer_focus_lost(ONLY IN_THREAD,
						       const callback_trigger_t
						       &trigger)
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
			trigger);
}

void generic_windowObj::handlerObj::update_frame_extents(ONLY IN_THREAD)
{
#ifdef UPDATE_DEST_METRICS_RECEIVED
	UPDATE_DEST_METRICS_RECEIVED();
#endif
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

void generic_windowObj::handlerObj
::set_window_type_in_constructor(const std::string_view &type)
{
	mpobj<ewmh>::lock lock{screenref->get_connection()->impl->ewmh_info};

	lock->set_window_type(id(), type);
}

void generic_windowObj::handlerObj
::set_window_state_in_constructor(const std::string_view &state)
{
	mpobj<ewmh>::lock lock(screenref->get_connection()->impl->ewmh_info);

	lock->set_state(id(), state);
}

void generic_windowObj::handlerObj
::set_window_type(const std::string_view &s)
{
	// This is done in the connection thread in order to ensure a
	// flush.

	thread()->run_as([s=std::string{s.begin(), s.end()},
			  me=ref<generic_windowObj::handlerObj>(this)]
			 (ONLY IN_THREAD)
			 {
				 me->set_window_type(IN_THREAD, s);
			 });
}

void generic_windowObj::handlerObj
::set_window_type(ONLY IN_THREAD, const std::string_view &s)
{
	set_window_type_in_constructor(s);
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
// Inherited from gcObj::handlerObj

drawableObj::implObj &generic_windowObj::handlerObj::get_drawable_impl()
{
	return *this;
}

const drawableObj::implObj &generic_windowObj::handlerObj::get_drawable_impl()
	const
{
	return *this;
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
	return mpobj<rectangle>::lock(current_position)->height;
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

void focusableObj::focusable_receive_selection()
{
	focusable_receive_selection(get_impl()->get_focusable_element()
				    .default_cut_paste_selection());
}

void focusableObj
::focusable_receive_selection(const std::string_view &selection)
{
	get_impl()->get_focusable_element().get_window_handler()
		.thread()->run_as
		([me=ref{this},
		  selection=std::string{selection}]
		 (ONLY IN_THREAD)
		 {
			 me->focusable_receive_selection(IN_THREAD, selection);
		 });
}

void focusableObj
::focusable_receive_selection(ONLY IN_THREAD, const std::string_view &selection)
{
	auto impl=get_impl();
	auto &wh=impl->get_focusable_element().get_window_handler();

	auto focusable=wh.get_autorestorable_focusable();

	if (focusable != impl)
		return;

	wh.receive_selection(IN_THREAD, selection);
}

void generic_windowObj::handlerObj
::receive_selection(ONLY IN_THREAD,
		    const std::string_view &selection)
{
	auto selection_atom=IN_THREAD->info->get_atom(selection);

	if (selection_atom == XCB_NONE)
		return;

	receive_selection(IN_THREAD, selection_atom);
}

void generic_windowObj::handlerObj
::receive_selection(ONLY IN_THREAD,
		    xcb_atom_t selection)
{
	const auto timestamp=IN_THREAD->timestamp(IN_THREAD);

	auto handler=current_selection_paste_handler
		::create(std::vector<xcb_atom_t>{
				{IN_THREAD->info->atoms_info.string}});

	if (convert_selection(IN_THREAD, selection,
			      IN_THREAD->info->atoms_info.cxxwpaste,
			      IN_THREAD->info->atoms_info.utf8_string,
			      timestamp))
	{
		clipboard_being_pasted(IN_THREAD)=selection;
		clipboard_paste_timestamp(IN_THREAD)=timestamp;
		conversion_handler(IN_THREAD)=handler;
	}

}

void generic_windowObj::handlerObj
::conversion_failed(ONLY IN_THREAD, xcb_atom_t type)
{
	auto handler=conversion_handler(IN_THREAD);

	if (!handler)
		return;

	conversion_handler(IN_THREAD)=nullptr;

	handler->conversion_failed(IN_THREAD, type, *this);
}

void generic_windowObj::handlerObj
::property_notify_event(ONLY IN_THREAD,
			const xcb_property_notify_event_t *msg)
{
	if (msg->atom == IN_THREAD->info->atoms_info.net_frame_extents)
	{
		update_frame_extents(IN_THREAD);
		return;
	}

	window_handlerObj::property_notify_event(IN_THREAD, msg);
}

bool generic_windowObj::handlerObj
::begin_converted_data(ONLY IN_THREAD, xcb_atom_t type,
		       xcb_timestamp_t timestamp)
{
	if (!conversion_handler(IN_THREAD))
		return false;

	return conversion_handler(IN_THREAD)->begin_converted_data(IN_THREAD,
								   type,
								   timestamp);
}

void generic_windowObj::handlerObj
::converted_data(ONLY IN_THREAD, xcb_atom_t clipboard,
		 xcb_atom_t type,
		 xcb_atom_t format,
		 void *data,
		 size_t size)
{
	if (!conversion_handler(IN_THREAD))
		return;

	conversion_handler(IN_THREAD)->converted_data(IN_THREAD, data, size,
						      *this);
}

void generic_windowObj::handlerObj
::end_converted_data(ONLY IN_THREAD)
{
	auto handler=conversion_handler(IN_THREAD);

	if (!handler)
		return;

	conversion_handler(IN_THREAD)=nullptr;

	handler->end_converted_data(IN_THREAD, *this);
}

void generic_windowObj::handlerObj
::pasted_string(ONLY IN_THREAD, const std::u32string_view &s)
{
	auto focusable=get_autorestorable_focusable();

	if (focusable)
		focusable->get_focusable_element().pasted(IN_THREAD, s);
}

bool generic_windowObj::handlerObj
::cut_or_copy_selection(cut_or_copy_op op,
			const std::string_view &selection)
{
	auto focusable=get_autorestorable_focusable();
	auto selection_atom=thread()->info->get_atom(selection);

	return focusable && selection_atom != XCB_NONE &&
		focusable->get_focusable_element()
		.cut_or_copy_selection(op, selection_atom);
}

bool generic_windowObj::handlerObj
::cut_or_copy_selection(ONLY IN_THREAD, cut_or_copy_op op,
			const std::string_view &selection)
{
	auto focusable=get_autorestorable_focusable();
	auto selection_atom=IN_THREAD->info->get_atom(selection);

	return focusable && selection_atom != XCB_NONE &&
		focusable->get_focusable_element()
		.cut_or_copy_selection(IN_THREAD, op, selection_atom);
}

bool focusableObj
::focusable_cut_or_copy_selection(cut_or_copy_op op)
{
	return focusable_cut_or_copy_selection
		(op,
		 get_impl()->get_focusable_element()
		 .default_cut_paste_selection());
}

bool focusableObj
::focusable_cut_or_copy_selection(cut_or_copy_op op,
				  const std::string_view &selection)
{
	auto impl=get_impl();
	auto &wh=impl->get_focusable_element().get_window_handler();

	auto focusable=wh.get_autorestorable_focusable();
	auto selection_atom=wh.thread()->info->get_atom(selection);

	return focusable == impl && selection_atom != XCB_NONE &&
		focusable->get_focusable_element()
		.cut_or_copy_selection(op, selection_atom);
}

bool focusableObj
::focusable_cut_or_copy_selection(ONLY IN_THREAD,
				  cut_or_copy_op op)
{
	return focusable_cut_or_copy_selection
		(IN_THREAD, op,
		 get_impl()->get_focusable_element()
		 .default_cut_paste_selection());
}

bool focusableObj
::focusable_cut_or_copy_selection(ONLY IN_THREAD,
				  cut_or_copy_op op,
				  const std::string_view &selection)
{
	auto impl=get_impl();
	auto &wh=impl->get_focusable_element().get_window_handler();

	auto focusable=wh.get_autorestorable_focusable();
	auto selection_atom=wh.thread()->info->get_atom(selection);

	return focusable == impl && selection_atom != XCB_NONE &&
		focusable->get_focusable_element()
		.cut_or_copy_selection(IN_THREAD, op, selection_atom);
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

font_arg generic_windowObj::handlerObj::label_theme_font() const
{
	return appearance->label_font;
}

color_arg generic_windowObj::handlerObj::label_theme_color() const
{
	return appearance->label_color;
}

LIBCXXW_NAMESPACE_END
