/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_handler_h
#define main_window_handler_h

#include "generic_window_handler_resources.H"
#include "main_window.H"
#include "connection_threadfwd.H"
#include "x/w/busyfwd.H"
#include <xcb/xcb_icccm.h>

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class LIBCXX_HIDDEN main_windowObj::handlerObj :
	public generic_windowObj::handlerObj::resourcesObj {

	std::function<void (const busy &)> on_delete_callback_thread_only;

 public:

	THREAD_DATA_ONLY(on_delete_callback);

	handlerObj(IN_THREAD_ONLY, const screen &parent_screen,
		   const color_arg &background_color);

	//! Destructor
	~handlerObj();

	void client_message_event(IN_THREAD_ONLY,
				  const xcb_client_message_event_t *event)
		override;

	//! Override horizvert_updated()

	//! Invoke update_size_hints() if we are visible.
	void horizvert_updated(IN_THREAD_ONLY) override;

	//! Override set_inherited_visibility().

	//! Invoke update_size_hints() before invoking the superclass if
	//! the window is made visible.

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info &);

	//! Invoke compute_size_hints(), then put them into WM_SIZE_HINTS.

	//! Our metrics go into WM_SIZE_HINTS, for the window manager to
	//! resize us, if we are visible.

	void update_size_hints(IN_THREAD_ONLY);

	//! Translate our metrics into WM_SIZE_HINTS
	virtual xcb_size_hints_t compute_size_hints(IN_THREAD_ONLY);

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
