/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_slider_focusframe_h
#define pane_slider_focusframe_h

#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "cursor_pointer_elementfwd.H"
#include "panelayoutmanager/pane_slider_original_sizes.H"
#include "x/w/pane_layout_appearancefwd.H"
#include "x/w/rgb.H"
#include <optional>

LIBCXXW_NAMESPACE_START

//! Focus frame around the slider in the pane.

//! It's better user experience to intercept keyboard and button events
//! in the wider space that encloses the focus frame around the slider,
//! and also install the slider's custom pointer image for the entire
//! area, rather than have the slider respond only when the pointer is
//! inside the focus frame and on top of the slider element.

class LIBCXX_HIDDEN pane_slider_focusframeObj
	: public cursor_pointer_elementObj<always_visible_focusframe_t> {

	//! Alias.
	typedef cursor_pointer_elementObj<always_visible_focusframe_t
					  > superclass_t;

	//! Pointer location when button 1 was pressed.
	coord_t grabbed_x;

	//! Pointer location when button 1 was pressed.
	coord_t grabbed_y;

	//! Captured original sizes
	std::optional<pane_slider_original_sizes> original_sizes;

 public:
	pane_slider_focusframeObj(const container_impl &parent,
				  const const_pane_layout_appearance
				  &appearance,
				  const cursor_pointer &custom_pointer);

	~pane_slider_focusframeObj();

	//! Override initialize()
	void initialize(ONLY IN_THREAD) override;

	//! Override theme_updated()
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;

	//! Override process_button_event().

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Override process_key_event().

	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	//! Override report_motion_event().

	void report_motion_event(ONLY IN_THREAD, const motion_event &) override;

	//! Override pointer_focus()

	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger) override;

};

LIBCXXW_NAMESPACE_END

#endif
