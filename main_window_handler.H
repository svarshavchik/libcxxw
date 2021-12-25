/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_handler_h
#define main_window_handler_h

#include "generic_window_handler.H"
#include "main_window.H"
#include "window_handler_constructor_params.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/main_window_appearancefwd.H"
#include "x/w/screen_positionsfwd.H"
#include "icon_images_vector_elementfwd.H"
#include "x/w/busyfwd.H"
#include "sync_counter.H"
#include <xcb/xcb_icccm.h>
#include <vector>
#include <string>

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class main_windowObj::handlerObj :
	public icon_images_vector_elementObj<generic_windowObj::handlerObj> {

	typedef icon_images_vector_elementObj<
		generic_windowObj::handlerObj> superclass_t;

	functionref<void (THREAD_CALLBACK,
			  const busy &)> on_delete_callback_thread_only;

	//! Whether an icon was set.
	bool wm_icon_set_thread_only=false;

	//!  _NET_WM_SYNC_REQUEST counter

	sync_counter net_wm_sync_request_counter;

	//! Received a _NET_WM_SYNC_REQUEST
	std::optional<int64_t> reconfigure_sync_request_received_thread_only;

	//! Processed a _NET_WM_SYNC_REQUEST
	std::optional<int64_t> reconfigure_sync_request_processed_thread_only;

public:
	THREAD_DATA_ONLY(reconfigure_sync_request_received);
	THREAD_DATA_ONLY(reconfigure_sync_request_processed);
	THREAD_DATA_ONLY(on_delete_callback);
	THREAD_DATA_ONLY(wm_icon_set);

	//! Alias
	typedef main_window_handler_constructor_params constructor_params;

	//! Constructor
	handlerObj(const constructor_params &params,
		   const std::optional<rectangle> &suggested_position,
		   const std::string &window_id);

	//! Destructor
	~handlerObj();

	//! Register a top-level window

	//! Overridden in dialog_handlerObj.

	virtual void register_current_main_window();

	//! Override installed().

	void installed(ONLY IN_THREAD) override;

	//! Weak pointer to our public object. Initialized during construction.

	weakptr<ptr<main_windowObj>> public_object;

	//! Return my public object.

	main_windowptr get_main_window() override;

	//! Implement client_message_event.
	void client_message_event(ONLY IN_THREAD,
				  const xcb_client_message_event_t *event)
		override;

	//! Override horizvert_updated()

	//! Invoke publish_size_hints() if we are visible.
	void horizvert_updated(ONLY IN_THREAD) override;

	//! Override install_size_hints

	//! Installs current_size_hints
	void install_size_hints(const size_hints &hints) override;

	//! Override size_hints_updated

	//! Invokes publish_size_hints.
	void size_hints_updated(ONLY IN_THREAD) override;

	//! Implement set_default_wm_hints().
	void set_default_wm_hints(ONLY IN_THREAD,
				  xcb_icccm_wm_hints_t &hints) override;

	//! Thread-safe container for current size hints.
	typedef mpobj<weakptr<size_hintsptr>> current_size_hints_t;

	//! Current hints installed in this window
		current_size_hints_t current_size_hints;

	//! Override set_inherited_visibility().

	//! Invoke publish_size_hints() before invoking the superclass if
	//! the window is made visible.

	void set_inherited_visibility(ONLY IN_THREAD,
				      inherited_visibility_info &) override;

	//! Invoke compute_size_hints(), then put them into WM_SIZE_HINTS.

	//! Our metrics go into WM_SIZE_HINTS, for the window manager to
	//! resize us, if we are are ok_to_publish_hints.

	void publish_size_hints(ONLY IN_THREAD);

	//! Override update_resizing_timeout().

	//! Until the first time this main_window gets shown, we need to
	//! override our defaults assumption as to whether the window manager
	//! will resize us. It doesn't pay attention until it's time to
	//! show us.
	void update_resizing_timeout(ONLY IN_THREAD) override;

 private:
	//! Recommended screen position for this window.

	std::optional<rectangle> suggested_position_thread_only;
 public:
	THREAD_DATA_ONLY(suggested_position);

	//! My appearance
	const const_main_window_appearance appearance;

	//! Unique identifier for the window.
	const std::string window_id;

	//! Retrieve hierarchical window identifier

	//! The default implementation returns window_id. A dialog handler
	//! overrides this, calls the parent handler's window_id_hierarchy()
	//! then this one. This populates the parameter with the hierarchical
	//! list of window_id from top level ids.

	virtual void window_id_hierarchy(std::vector<std::string> &)
		const override;

	//! Translate our metrics into WM_SIZE_HINTS
	virtual xcb_size_hints_t compute_size_hints(ONLY IN_THREAD);

	//! Override request_visiblity()

	//! Set window size to preferred size before its visible for 1st time.

	void request_visibility(ONLY IN_THREAD, bool flag) override;

	//! Override frame_extents_updated()

	//! Trigger the top level's layout manager's recalculation after the
	//! frame extents got updated. The toplevel layout manager, a peephole
	//! May need to recalculate itself.

	void frame_extents_updated(ONLY IN_THREAD) override;

 private:
	//! The first time the window is mapped, it's resized to its preferred size, first.

	bool preferred_dimensions_set_thread_only=false;

	//! And once that's done, we're ok to publish size hints.
	bool ok_to_publish_hints_thread_only=false;

	//! Determine what the initial window size should be.
	std::tuple<dim_t, dim_t> compute_initial_size(ONLY IN_THREAD);

	//! Check if we're ok to publish the initial window size.
	void update_initial_size(ONLY IN_THREAD,
				 dim_t, dim_t,
				 size_t);

	//! Preferred width
	dim_t preferred_width_thread_only;

	//! Preferred height
	dim_t preferred_height_thread_only;

	//! Update the _NET_WM_ICON property

	void update_net_wm_icon(ONLY IN_THREAD);

 public:

	//! Override configure_notify_received()

	//! Until preferred_dimensions_set(), we think that our size is 0.
	//! However because X11 proto throws a red flag if you attempt to
	//! size a window to 0 width or depth, we will set it to 1x1, however
	//! we still need to think that the window size is (0, 0), for our
	//! purposes.

	void configure_notify_received(ONLY IN_THREAD, const rectangle &r)
		override;

	//! Install new icons.
	void install_window_icons(const std::vector<std::string> &icons);

	//! Install new icons.
	void install_window_icons(const std::string &icons);

	//! Install new icons.
	void install_window_icons(ONLY IN_THREAD,
				  const std::vector<icon> &icons);

	//! Override theme_updated.

	//! Invoke update_net_wm_icon();

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;

	THREAD_DATA_ONLY(preferred_dimensions_set);
	THREAD_DATA_ONLY(ok_to_publish_hints);
	THREAD_DATA_ONLY(preferred_width);
	THREAD_DATA_ONLY(preferred_height);

	//! Override process_configure_notify

	void process_configure_notify(ONLY IN_THREAD) override;

	//! Override idle().

	//! Check if reconfigure_sync_request_processed,
	void idle(ONLY IN_THREAD) override;

	//! Override will_accept_transferred_focus()

	//! Main windows (and dialogs) accept keyboard focus after
	//! tabbing out of another window, as long as we're not is_input_busy()
	//! and we are visible.

	bool will_accept_transferred_focus(ONLY IN_THREAD) override;
};

LIBCXXW_NAMESPACE_END

#endif
