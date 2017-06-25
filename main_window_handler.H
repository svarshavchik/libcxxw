/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_handler_h
#define main_window_handler_h

#include "generic_window_handler.H"
#include "main_window.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class LIBCXX_HIDDEN main_windowObj::handlerObj :
	public generic_windowObj::handlerObj {

	std::function<void ()> on_delete_callback_thread_only;

 public:

	THREAD_DATA_ONLY(on_delete_callback);

	handlerObj(IN_THREAD_ONLY, const screen &parent_screen);

	//! Destructor
	~handlerObj();

	void client_message_event(IN_THREAD_ONLY,
				  const xcb_client_message_event_t *event)
		override;

	//! Override horizvert_updated()

	//! The metrics go into WM_SIZE_HINTS, for the window manager to
	//! resize us.
	void horizvert_updated(IN_THREAD_ONLY) override;

	//! Override request_visiblity()

	//! Set window size to preferred size before its visible for 1st time.

	void request_visibility(IN_THREAD_ONLY, bool flag) override;

	//! Override frame_extents_updated()

	//! Trigger the top level's layout manager's recalculation after the
	//! frame extents got updated. The toplevel layout manager, a peephole
	//! May need to recalculate itself.

	void frame_extents_updated(IN_THREAD_ONLY) override;

 private:
	//! The first time the window is mapped, it's resized to its preferred size, first.

	bool preferred_dimensions_set_thread_only=false;

	//! Preferred width
	dim_t preferred_width_thread_only;

	//! Preferred height
	dim_t preferred_height_thread_only;

 public:
	THREAD_DATA_ONLY(preferred_dimensions_set);
	THREAD_DATA_ONLY(preferred_width);
	THREAD_DATA_ONLY(preferred_height);
};

LIBCXXW_NAMESPACE_END

#endif
