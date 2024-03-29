/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef dialog_handler_h
#define dialog_handler_h

#include "x/w/dialogobj.H"
#include "x/w/dialogfwd.H"
#include "x/w/rgbfwd.H"
#include "x/w/generic_window_appearancefwd.H"
#include "main_window_handler.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/screen_positionsfwd.H"
#include "x/w/impl/screen_positions_confighandlefwd.H"
#include <optional>

LIBCXXW_NAMESPACE_START

//! Internal handler.

class dialogObj::handlerObj : public main_windowObj::handlerObj {

	//! Alias

	typedef main_windowObj::handlerObj superclass_t;

	//! If modal, and visible, the acquired busy mcguffin.

	ptr<obj> acquired_busy_mcguffin_thread_only;

	//! My position with respect to the parent

	dialog_position my_position_thread_only=
		dialog_position::default_position;
 public:
	THREAD_DATA_ONLY(acquired_busy_mcguffin);
	THREAD_DATA_ONLY(my_position);

	//! Whether this is a modal dialog.

	const bool modal;

	//! Parent display element.
	const ref<main_windowObj::handlerObj> parent_handler;

	//! Constructor

	handlerObj(const ref<main_windowObj::handlerObj> &parent_handler,
		   const dialog_position &position,
		   const std::optional<rectangle> &initial_pos,
		   const std::string &window_id,
		   const screen_positions &positions,
		   const screen_positions_handle &config_handle,
		   const color_arg &background_color,
		   const const_main_window_appearance &appearance,
		   const char *window_type,
		   bool modal,
		   bool urgent,
		   bool grab_input_focus);

	//! Override window_id_hierarchy().

	void window_id_hierarchy(std::vector<std::string> &) const override;

	//! Override register_current_main_window()

	void register_current_main_window() override;

	//! Override set_default_wm_hints().

	void set_default_wm_hints(ONLY IN_THREAD,
				  xcb_icccm_wm_hints_t &hints) override;

	//! Override button_press_event

	//! Turn off the urgent flag.
	void button_press_event(ONLY IN_THREAD,
				const xcb_button_press_event_t *event) override;

	//! Override key_press_event

	//! Turn off the urgent flag.
	void key_press_event(ONLY IN_THREAD,
			     const xcb_key_press_event_t *event,
			     uint16_t sequencehi) override;
private:
	//! This is an urgent dialog
	const bool urgent_dialog;

	//! Grab input focus before mapping
	const bool grab_input_focus;

	//! This dialog is in urgent state.

	//! Set to urgent_dialog when the dialog gets mapped. Turned off by
	//! the first keypress or button click.

	bool urgent_thread_only;

public:
	THREAD_DATA_ONLY(urgent);

	//! Override set_inherited_visibility_mapped)(.

	//! If we need to handle_our_own_placement(), figure out what it
	//! should be, if we're made visible.
	//!
	//! Also call our parent's shared handler data's opening_dialog().
	//!
	//! If modal, acquire the busy mcguffin, accordingly.

	void set_inherited_visibility_mapped(ONLY IN_THREAD) override;

	//! Override set_inherited_visibility_unmapped().

	//! Calls our parent's shared handler data's closing_dialog().
	//!
	//! If modal, release the busy mcguffin, accordingly.

	void set_inherited_visibility_unmapped(ONLY IN_THREAD) override;

	//! Override compute_size_hints.

	//! If we need to handle_our_own_placement() then we set the P_POSITION
	//! hint (and set_inherited_visibility() will calculate what it is).

	xcb_size_hints_t compute_size_hints(ONLY IN_THREAD) override;

private:

	//! Check if the window manager support _NET_WM_FULL_PLACEMENT, or not.

	//! This may not be the only reason we want to set our own position,
	//! it also depends on my_position, too.
	bool handle_our_own_placement(ONLY IN_THREAD);

public:
	//! Destructor
	~handlerObj();
};

LIBCXXW_NAMESPACE_END

#endif
