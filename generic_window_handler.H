/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_handler_h
#define generic_window_handler_h

#include "window_handler.H"
#include "generic_window.H"
#include "drawable.H"
#include "picture.H"
#include "connection_threadfwd.H"
#include "background_colorobj.H"
#include "x/w/pictformatfwd.H"
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class LIBCXX_HIDDEN generic_windowObj::handlerObj

// The window handler
	: public window_handlerObj,

// The window's drawable
	  public drawableObj::implObj,

// The picture for the window
	  public pictureObj::implObj::fromDrawableObj,
	  public background_colorObj {

	xcb_event_mask_t current_events_thread_only;

 public:
	//! What I want for my initial event mask.

	static constexpr xcb_event_mask_t initial_event_mask()
	{
		return (xcb_event_mask_t)(XCB_EVENT_MASK_EXPOSURE |
					  XCB_EVENT_MASK_VISIBILITY_CHANGE |
					  XCB_EVENT_MASK_STRUCTURE_NOTIFY);
	}

	//! What my constructor needs

	struct constructor_params {

		//! Parameters for window_handler's constructor.

		window_handlerObj::constructor_params window_handler_params;

		//! The window's pictformat.

		const_pictformat drawable_pictformat;

		//! The window's initial background color

		background_colorObj
		::background_color_closure_t initial_background_color;
	};

	handlerObj(IN_THREAD_ONLY,
		   const constructor_params &params);

	//! Destructor
	~handlerObj();

	THREAD_DATA_ONLY(current_events);

	//! Create the window's PICTURE.

	//! This gets called after a window is created, immediately after
	//! xcb_create_window()

	void create_picture(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
