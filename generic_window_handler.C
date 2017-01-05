/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "pictformat.H"

LIBCXXW_NAMESPACE_START

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

	background_colorObj(params.initial_background_color)
{
	current_events(IN_THREAD)=
		(xcb_event_mask_t)
		params.window_handler_params
		.events_and_mask.m.at(XCB_CW_EVENT_MASK);
}

generic_windowObj::handlerObj::~handlerObj()=default;

void generic_windowObj::handlerObj::create_picture(IN_THREAD_ONLY)
{
}


LIBCXXW_NAMESPACE_END
