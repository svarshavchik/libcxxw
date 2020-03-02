/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_child_elementobj_h
#define x_w_impl_child_elementobj_h

#include <x/w/impl/element.H>
#include <x/w/impl/child_elementfwd.H>
#include <x/w/containerobj.H>
#include <x/w/impl/background_color_elementfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/impl/focus/focusablefwd.H>
#include <x/w/impl/popup/popupfwd.H>
#include <x/w/impl/popup/popupobj.H>
#include <x/w/metrics/axis.H>
#include <x/w/picturefwd.H>
#include <x/w/rgb.H>

LIBCXXW_NAMESPACE_START

//! Additional constructor parameters

struct child_element_init_params {

	//! The ID for the element's scratch buffer.
	std::string scratch_buffer_id;

	//! Element's initial metrics
	metrics::horizvert_axi initial_metrics;

	//! Element's background color
	std::optional<color_arg> background_color;

	//! Element's attached popup
	popupptr attached_popup;
};

//! Tag for the background color

struct child_element_bgcolor;

//! An element that has a parent widget.

class child_elementObj
	: public background_color_elementObj<elementObj::implObj,
					     child_element_bgcolor> {

	//! Alias
	typedef background_color_elementObj<elementObj::implObj,
					    child_element_bgcolor> superclass_t;

	//! Returned by has_own_background_color()

	bool is_mine_background_color;

 public:

	//! Override background_color_changed

	//! Updates cached_draw_info, if needed.
	void background_color_changed(ONLY IN_THREAD) override;

private:
	//! After change in visibility update our cached background color

	void update_draw_info_background_color(ONLY IN_THREAD);
public:
	//! My parent widget
	const container_impl child_container;

	//! Constructor.

	//! Save parent element, increase this element's nesting level.

	child_elementObj(const container_impl &container);

	//! Constructor

	//! Additional constructor parameters
	child_elementObj(const container_impl &container,
			 const child_element_init_params &init_params);

	//! Destructor
	~child_elementObj();

	//////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj:

	//! Redraw my container after my position is updated.

	//! The container is responsible for clearing parts of it that are
	//! not covered by any widget.

	void process_updated_position(ONLY IN_THREAD) override;

	//! Implement get_parent_element_impl();

	implObj *get_parent_element_impl() const override;

	//! Use parent widget's background color.

	void remove_background_color(ONLY IN_THREAD) override;

	//! Use its own background color.

	void set_background_color(ONLY IN_THREAD,
				  const background_color &) override;

	//! Theme has changed.

	//! If this child widget has its own background color, if it was
	//! changed by the theme we pretend that set_background_color()
	//! has happened.

	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;

	//! Override schedule_hover_action

	//! This gets invoked for the widget under the pointer. Propagate
	//! this to its parent container, so that anything under the pointer
	//! with a tooltip will schedule its creation.

	void schedule_hover_action(ONLY IN_THREAD) override;

	//! Override schedule_hover_action

	//! This gets invoked for the widget under the pointer. Propagate
	//! this to its parent container, so that anything under the pointer
	//! with a tooltip will schedule its creation.
	void unschedule_hover_action(ONLY IN_THREAD) override;

	//! Forward the call to the container.
	void window_focus_change(ONLY IN_THREAD, bool flag) override;

	//! If child_element doesn't handle it, punt it back to the parent.
	bool process_key_event(ONLY IN_THREAD, const key_event &) override;

	//! If the parent container is set as disabled, so are we.

	bool enabled(ONLY IN_THREAD) override;

	//! Override draw_to_window_picture_as_disabled.

	bool draw_to_window_picture_as_disabled(ONLY IN_THREAD) override;

	//! Button events

	//! All widgets, starting with the one under the pointer
	//! and its parents, get notified of a button event. The return
	//! value is true if anyone returned true.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Implement grab().

	void grab(ONLY IN_THREAD) override;

	//! Motion events

	//! The motion event, after it's reported for this element,
	//! gets also reported for its parent widget.

	void report_motion_event(ONLY IN_THREAD, const motion_event &)
		override;

	//! Request visibility change.
	void request_visibility(ONLY IN_THREAD, bool flag) override;

 protected:

	//! Changed visibility mean a change to the background color.

	void set_inherited_visibility(ONLY IN_THREAD,
				      inherited_visibility_info
				      &visibility_info) override LIBCXX_PUBLIC;

	//! Whether set_background_color() is in force.

	bool has_own_background_color(ONLY IN_THREAD) override;

	//! Implement current_background_color()
	background_color current_background_color(ONLY IN_THREAD) override;

	//! Implement get_draw_info()
	draw_info &get_draw_info(ONLY IN_THREAD) override;

	//! Implement get_draw_info_from_scratch()
	draw_info &get_draw_info_from_scratch(ONLY IN_THREAD) override;

	//! Invoke parent_element->get_absolute_location(), and add my x/y
	rectangle get_absolute_location(ONLY IN_THREAD) const override;

	//! update_visibility() invokes this callback.

	//! This element's inherited visibility is visible only if
	//! both this widget is explicitly visible, and its parent is.

	void visibility_updated(ONLY IN_THREAD, bool flag) override;

 public:

	//! Input focus processing.

	//! Forward to parent.

	void focusable_initialized(ONLY IN_THREAD,
				   focusableObj::implObj &fimpl) override;

	//! Forward to parent.

	void get_focus_first(ONLY IN_THREAD, const focusable &f) override;

	//! Input focus processing.

	//! See element_focusable.C
	void requested_focus_to(ONLY IN_THREAD,
				const ptr<elementObj::implObj> &current_focus)
		override;

	//! Input focus processing.

	//! See element_focusable.C
	void requested_focus_from(ONLY IN_THREAD) override;

	//! Input focus processing.

	//! See element_focusable.C
	void do_leaving_focus(ONLY IN_THREAD,
			      focus_change &event,
			      const ref<implObj> &focus_from,
			      const ptr<elementObj::implObj> &leaving_for,
			      focus_reporter_t focus_reporter,
			      const callback_trigger_t &trigger) override;

	//! Input focus processing.

	//! See element_focusable.C
	void do_entering_focus(ONLY IN_THREAD,
			       focus_change event,
			       const ref<implObj> &focus_to,
			       const ptr<elementObj::implObj> &focus_from,
			       focus_reporter_t focus_reporter,
			       const callback_trigger_t &trigger) override;

	//! Input focus processing.

	//! See element_focusable.C

	void focus_movement_complete(ONLY IN_THREAD,
				     bool stop_at_original_focus,
				     focus_reporter_t focus_reporter,
				     const callback_trigger_t &trigger)
		override;

	//! Override ensure_visibility.

	void ensure_visibility(ONLY IN_THREAD, const rectangle &) override;

 protected:
	/////////////////////////////////////////////////////////////////////
	//
	// Inherited from horizvertObj

	void horizvert_updated(ONLY IN_THREAD) override;

 public:

	//! Received pasted text.

	//! Forwarded to the container, if not handled by the widget.

	bool pasted(ONLY IN_THREAD,
		    const std::u32string_view &str) override;

	//! Override creating_focusable_element().

	void creating_focusable_element() override;

	//! Override label_theme_font()

	font_arg label_theme_font() const override;

	//! Override label_theme_color()

	color_arg label_theme_color() const override;

	//! Override get_cursor_pointer().

	//! If this widget does not have a set cursor pointer,
	//! the call is forwarded to the parent.

	cursor_pointerptr get_cursor_pointer(ONLY IN_THREAD) override;

	//! Override element_name().

	//! Include the parent name.
	void element_name(std::ostream &) override;


	//! Forward find_acceptable_drop() to the parent container.

	//! This widget does not, so pass the buck to the container.
	bool find_acceptable_drop(ONLY IN_THREAD,
				  element_impl &accepting_element,
				  const source_dnd_formats_t &source_formats,
				  xcb_timestamp_t) override;

};

LIBCXXW_NAMESPACE_END

#endif
