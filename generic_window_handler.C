/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "pictformat.H"
#include "draw_info.H"
#include "container.H"
#include "layoutmanager.H"
#include "screen.H"
#include "xid_t.H"
#include "element_screen.H"
#include "values_and_mask.H"
#include <xcb/xcb_icccm.h>
LIBCXXW_NAMESPACE_START

#define ELEMENT_SUBCLASS generic_windowObj::handlerObj

#include "container_element_overrides_impl.H"

static rectangle element_position(const rectangle &r)
{
	auto cpy=r;

	cpy.x=0;
	cpy.y=0;
	return cpy;
}

static const_picture default_background_color(const screen &s)
{
	return s->create_solid_color_picture(rgb(0xCCCC, 0xCCCC, 0xCCCC));
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

       elementObj::implObj(0,
			   element_position(params.window_handler_params
					    .initial_position)),
	current_events_thread_only((xcb_event_mask_t)
				   params.window_handler_params
				   .events_and_mask.m.at(XCB_CW_EVENT_MASK)),
	current_position_thread_only(params.window_handler_params
				     .initial_position),
	background_color_thread_only(default_background_color
				     (params.window_handler_params.screenref))
{
}

generic_windowObj::handlerObj::~handlerObj()=default;

////////////////////////////////////////////////////////////////////
//
// Inherited from containerObj::implObj

elementObj::implObj &generic_windowObj::handlerObj::get_element_impl()
{
	return *this;
}

const elementObj::implObj &generic_windowObj::handlerObj::get_element_impl()
	const
{
	return *this;
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

draw_info generic_windowObj::handlerObj
::get_draw_info(IN_THREAD_ONLY,
		const rectangle &initial_viewport)
{
	return draw_info{
		        picture_internal(this),
			initial_viewport,
			background_color(IN_THREAD)->impl,
			0,
			0,
	};
}

void generic_windowObj::handlerObj
::draw_after_visibility_updated(IN_THREAD_ONLY, bool flag)
{
}

void generic_windowObj::handlerObj
::set_inherited_visibility(IN_THREAD_ONLY, bool flag)
{
	if (flag)
		xcb_map_window(IN_THREAD->info->conn, id());
	else
		xcb_unmap_window(IN_THREAD->info->conn, id());

	elementObj::implObj::set_inherited_visibility(IN_THREAD, flag);
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

			 values_and_mask configure_window_vals
				 (XCB_CONFIG_WINDOW_WIDTH,
				  (dim_t::value_type)
				  me->preferred_width(IN_THREAD),
				  XCB_CONFIG_WINDOW_HEIGHT,
				  (dim_t::value_type)
				  me->preferred_height(IN_THREAD))
				 ;

			 xcb_configure_window(IN_THREAD->info->conn, me->id(),
					      configure_window_vals.mask(),
					      configure_window_vals.values()
					      .data());

			 // Also simulate a configure_notify(), so that the
			 // display elements get arranged accordingly right
			 // away. We don't know when we'll get back the
			 // ConfigureNotify message from the display server.

			 rectangle r{0, 0, me->preferred_width(IN_THREAD),
					 me->preferred_height(IN_THREAD)};

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
	background_color(IN_THREAD)=default_background_color(get_screen());
	schedule_redraw_if_visible(IN_THREAD);
}

void generic_windowObj::handlerObj::set_background_color(IN_THREAD_ONLY,
							 const const_picture &p)
{
	background_color(IN_THREAD)=p;
	schedule_redraw_if_visible(IN_THREAD);
}

///////////////////////////////////////////////////////////////////////////////
//
// Inherited from window_handler

void generic_windowObj::handlerObj::exposure_event(IN_THREAD_ONLY,
						   rectangle_set &areas)
{
	auto di=get_draw_info(IN_THREAD,
			      data(IN_THREAD).current_position);
	draw(IN_THREAD, di, areas);
}

void generic_windowObj::handlerObj::configure_notify(IN_THREAD_ONLY,
						     const rectangle &r)
{
	// x & y are the window's position on the script

	// for our purposes, the display element representing the top level
	// window's coordinates are (0, 0), so we update only the width and
	// the height.

	rectangle cpy=r;

	cpy.x=0;
	cpy.y=0;

	update_current_position(IN_THREAD, cpy);
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

LIBCXXW_NAMESPACE_END
