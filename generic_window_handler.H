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
#include "element.H"
#include "x/w/pictformatfwd.H"
#include "x/w/types.H"
#include "x/w/screenfwd.H"

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

// An element
	  public elementObj::implObj {

	//! Current event mask.
	xcb_event_mask_t current_events_thread_only;

	//! Current position and dimensions

	rectangle current_position_thread_only;

	//! Window's background color

	const_picture background_color_thread_only;

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
	};

	handlerObj(IN_THREAD_ONLY,
		   const constructor_params &params);

	//! Destructor
	~handlerObj();

	THREAD_DATA_ONLY(current_events);
	THREAD_DATA_ONLY(current_position);
	THREAD_DATA_ONLY(background_color);

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Return myself.
	generic_windowObj::handlerObj &get_window_handler() override;

	//! Return myself.
	const generic_windowObj::handlerObj &get_window_handler() const override;

	//! Construct a draw_info for drawing into the requested viewport.

	//! Give ourselves away as the picture to draw info, and use
	//! our declared background color.

	draw_info get_draw_info(IN_THREAD_ONLY,
				const rectangle &initial_viewport)
		override;
	////////////////////////////////////////////////////////////////////
	//
	// Inherited from window_handler

	//! Implement the exposure event.

	void exposure_event(IN_THREAD_ONLY,
			    std::set<rectangle> &areas) override;

	//! A visibility update is translated to mapping/unmapping the window.

	void visibility_updated(IN_THREAD_ONLY, bool flag) override;

};

LIBCXXW_NAMESPACE_END

#endif
