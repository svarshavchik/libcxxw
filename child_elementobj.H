/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef child_elementobj_h
#define child_elementobj_h

#include "element.H"
#include "container.H"
#include "background_color_elementfwd.H"
#include "connection_threadfwd.H"
#include "focus/focusablefwd.H"
#include "x/w/metrics/axis.H"
#include "x/w/picturefwd.H"
#include "x/w/rgb.H"

LIBCXXW_NAMESPACE_START

//! Additional constructor parameters

struct LIBCXX_HIDDEN child_element_init_params {

	//! The ID for the element's scratch buffer.
	std::string scratch_buffer_id;

	//! Element's initial metrics
	metrics::horizvert_axi initial_metrics;

	//! Element's background color
	std::optional<color_arg> background_color;
};

//! An element that has a parent element.

class LIBCXX_HIDDEN child_elementObj
	: public background_color_elementObj<elementObj::implObj> {

	//! Alias
	typedef background_color_elementObj<elementObj::implObj> superclass_t;

	//! Returned by has_own_background_color()

	bool is_mine_background_color;

 public:

	//! My parent element
	const ref<containerObj::implObj> child_container;

	//! Constructor.

	//! Save parent element, increase this element's nesting level.

	child_elementObj(const ref<containerObj::implObj> &container);

	//! Constructor

	//! Additional constructor parameters
	child_elementObj(const ref<containerObj::implObj> &container,
			 const child_element_init_params &init_params);

	//! Destructor
	~child_elementObj();

	//////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj:

	//! Invoke parent_element->get_window_handler().
	generic_windowObj::handlerObj &get_window_handler() override;

	//! Invoke parent_element->get_window_handler().
	const generic_windowObj::handlerObj &get_window_handler() const
		override;

	//! Redraw my container after my position is updated.

	//! The container is responsible for clearing parts of it that are
	//! not covered by any element.

	void process_updated_position(IN_THREAD_ONLY) override;

	//! Use parent display element's background color.

	void remove_background_color(IN_THREAD_ONLY) override;

	//! Use its own background color.

	void set_background_color(IN_THREAD_ONLY,
				  const background_color &) override;

	//! Override schedule_hover_action

	//! This gets invoked for the element under the pointer. Propagate
	//! this to its parent container, so that anything under the pointer
	//! with a tooltip will schedule its creation.

	void schedule_hover_action(IN_THREAD_ONLY) override;

	//! Override schedule_hover_action

	//! This gets invoked for the element under the pointer. Propagate
	//! this to its parent container, so that anything under the pointer
	//! with a tooltip will schedule its creation.
	void unschedule_hover_action(IN_THREAD_ONLY) override;

	//! Forward the call to the container.
	void window_focus_change(IN_THREAD_ONLY, bool flag) override;

	//! If child_element doesn't handle it, punt it back to the parent.
	bool process_key_event(IN_THREAD_ONLY, const key_event &) override;

	//! If the parent container is set as disabled, so are we.

	bool enabled(IN_THREAD_ONLY) override;

	//! Override draw_to_window_picture_as_disabled.

	bool draw_to_window_picture_as_disabled(IN_THREAD_ONLY) override;

	//! Button events

	//! All display elements, starting with the one under the pointer
	//! and its parents, get notified of a button event. The return
	//! value is true if anyone returned true.

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Implement grab().

	void grab(IN_THREAD_ONLY) override;

	//! Motion events

	//! The motion event, after it's reported for this element,
	//! gets also reported for its parent display element.

	void report_motion_event(IN_THREAD_ONLY, const motion_event &)
		override;

 protected:

	//! Changed visibility mean a change to the background color.

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info
				      &visibility_info) override;

	//! Whether set_background_color() is in force.

	bool has_own_background_color(IN_THREAD_ONLY) override;

	//! Implement current_background_color()
	background_color current_background_color(IN_THREAD_ONLY) override;

	//! Implement get_draw_info()
	draw_info &get_draw_info(IN_THREAD_ONLY) override;

	//! Implement get_draw_info_from_scratch()
	draw_info &get_draw_info_from_scratch(IN_THREAD_ONLY) override;

	//! Invoke parent_element->get_absolute_location(), and add my x/y
	rectangle get_absolute_location(IN_THREAD_ONLY) override;

	//! update_visibility() invokes this callback.

	//! This element's inherited visibility is visible only if
	//! both this element is explicitly visible, and its parent is.

	void visibility_updated(IN_THREAD_ONLY, bool flag) override;

 public:

	//! Input focus processing.

	//! Forward to parent.

	void focusable_initialized(IN_THREAD_ONLY,
				   focusableImplObj &fimpl) override;

	//! Forward to parent.

	void get_focus_first(IN_THREAD_ONLY, const focusable &f) override;

	//! Input focus processing.

	//! See element_focusable.C
	void requested_focus_to(IN_THREAD_ONLY,
				const ptr<elementObj::implObj> &current_focus)
		override;

	//! Input focus processing.

	//! See element_focusable.C
	void requested_focus_from(IN_THREAD_ONLY)
		override;

	//! Input focus processing.

	//! See element_focusable.C
	void do_leaving_focus(IN_THREAD_ONLY,
			      focus_change &event,
			      const ref<implObj> &focus_from,
			      const ptr<elementObj::implObj> &leaving_for,
			      focus_reporter_t focus_reporter,
			      const callback_trigger_t &trigger)
		override;

	//! Input focus processing.

	//! See element_focusable.C
	void do_entering_focus(IN_THREAD_ONLY,
			       focus_change event,
			       const ref<implObj> &focus_to,
			       const ptr<elementObj::implObj> &focus_from,
			       focus_reporter_t focus_reporter,
			       const callback_trigger_t &trigger)
		override;

	//! Input focus processing.

	//! See element_focusable.C

	void focus_movement_complete(IN_THREAD_ONLY,
				     bool stop_at_original_focus,
				     focus_reporter_t focus_reporter,
				     const callback_trigger_t &trigger)
		override;

	//! Override ensure_visibility.

	void ensure_visibility(IN_THREAD_ONLY, const rectangle &) override;

 protected:
	/////////////////////////////////////////////////////////////////////
	//
	// Inherited from horizvertObj

	void horizvert_updated(IN_THREAD_ONLY) override;

 public:

	//! Received pasted text.

	//! Forwarded to the container, if not handled by the element.

	bool pasted(IN_THREAD_ONLY,
		    const std::u32string_view &str) override;

	//! Override creating_focusable_element().

	void creating_focusable_element() override;

	//! Override label_theme_font()

	const char *label_theme_font() const override;

	//! Override label_theme_color()

	color_arg label_theme_color() const override;

	//! Override get_cursor_pointer().

	//! If this element does not have a set cursor pointer,
	//! the call is forwarded to the parent.

	cursor_pointerptr get_cursor_pointer(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
