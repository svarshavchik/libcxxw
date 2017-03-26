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
#include "container.H"
#include "layoutmanager.H"
#include "screen.H"
#include "xid_t.H"
#include "element_screen.H"
#include "background_color.H"
#include "x/w/values_and_mask.H"
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

       elementObj::implObj(0,
			   element_position(params.window_handler_params
					    .initial_position),
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
			viewport,
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
	if (visibility_info.flag)
	{
#ifdef MAP_LOG
		MAP_LOG();
#endif
		xcb_map_window(IN_THREAD->info->conn, id());
		visibility_info.do_not_redraw=true;
	}
	else
		xcb_unmap_window(IN_THREAD->info->conn, id());

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
						   rectangle_set &areas)
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
