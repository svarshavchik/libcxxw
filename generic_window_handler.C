/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "pictformat.H"
#include "draw_info.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

static rectangle element_position(const rectangle &r)
{
	auto cpy=r;

	cpy.x=0;
	cpy.y=0;
	return cpy;
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
	background_color_thread_only(params.window_handler_params
				     .screenref
				     ->create_solid_color_picture
				     (rgb(0xCCCC, 0xCCCC, 0xCCCC)))
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

///////////////////////////////////////////////////////////////////////////////
//
// Inherited from window_handler

void generic_windowObj::handlerObj::visibility_updated(IN_THREAD_ONLY,
						       bool flag)
{
	if (flag)
		xcb_map_window(IN_THREAD->info->conn, id());
	else
		xcb_unmap_window(IN_THREAD->info->conn, id());
}

void generic_windowObj::handlerObj::exposure_event(IN_THREAD_ONLY,
						   std::set<rectangle> &areas)
{
	auto di=get_draw_info(IN_THREAD,
			      data(IN_THREAD).current_position);
	clear_to_color(IN_THREAD, di, areas);
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

	current_position_updated(IN_THREAD, cpy);
}

LIBCXXW_NAMESPACE_END
