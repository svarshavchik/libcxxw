/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef child_elementobj_h
#define child_elementobj_h

#include "element.H"
#include "container.H"
#include "background_colorfwd.H"
#include "connection_threadfwd.H"
#include "focus/focusablefwd.H"
#include "x/w/metrics/axis.H"
#include "x/w/picturefwd.H"

LIBCXXW_NAMESPACE_START

//! An element that has a parent element.

class LIBCXX_HIDDEN child_elementObj : public elementObj::implObj {

	//! This display element's background color

	background_colorptr current_background_color_thread_only;

 public:

	THREAD_DATA_ONLY(current_background_color);

	//! My parent element
	const ref<containerObj::implObj> container;

	//! Constructor.

	//! Save parent element, increase this element's nesting level.
	//! scratch_buffer_id is forwarded to the element constructor.

	child_elementObj(const ref<containerObj::implObj> &container);

	//! Constructor, custom metrics and scratch buffer ID.
	child_elementObj(const ref<containerObj::implObj> &container,
			 const metrics::horizvert_axi &initial_metrics,
			 const std::string &scratch_buffer_id);

	//! Constructor, explicit background color.
	child_elementObj(const ref<containerObj::implObj> &container,
			 const metrics::horizvert_axi &initial_metrics,
			 const std::string &scratch_buffer_id,
			 const background_colorptr &initial_background_color);

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

	//! Forward the call to the container.
	void window_focus_change(IN_THREAD_ONLY, bool flag) override;

	//! If child_element doesn't handle it, punt it back to the parent.
	bool process_key_event(IN_THREAD_ONLY, const key_event &) override;

	//! Button events

	//! All display elements, starting with the one under the pointer
	//! and its parents, get notified of a button event. The return
	//! value is true if anyone returned true.

	bool process_button_event(IN_THREAD_ONLY,
				  int button,
				  bool press,
				  xcb_timestamp_t timestamp,
				  const input_mask &mask) override;

	//! Implement grab().

	void grab(IN_THREAD_ONLY) override;

	//! Motion events

	//! The motion event, after it's reported for this elemenbt,
	//! gets also reported for its parent display element.

	void motion_event(IN_THREAD_ONLY, coord_t x, coord_t y,
			  const input_mask &mask) override;

 protected:

	//! Update the theme-dependendent background color, then call the parent.

	void theme_updated(IN_THREAD_ONLY) override;

	//! Changed visibility mean a change to the background color.

	void set_inherited_visibility(IN_THREAD_ONLY,
				      inherited_visibility_info
				      &visibility_info) override;

	//! Whether set_background_color() is in force.

	bool has_own_background_color(IN_THREAD_ONLY) override;

	//! Invoke parent_element->draw_info(), and update the results.
	draw_info &get_draw_info(IN_THREAD_ONLY) override;

	//! Invoke parent_element->get_absolute_location(), and add my x/y
	rectangle get_absolute_location(IN_THREAD_ONLY) override;

	//! Set this child element's background color
	void prepare_draw_info(IN_THREAD_ONLY,
			       draw_info &) override;

	//! update_visibility() invokes this callback.

	//! This element's inherited visibility is visible only if
	//! both this element is explicitly visible, and its parent is.

	void visibility_updated(IN_THREAD_ONLY, bool flag) override;

 public:
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
			      focus_reporter_t focus_reporter)
		override;

	//! Input focus processing.

	//! See element_focusable.C
	void do_entering_focus(IN_THREAD_ONLY,
			       focus_change event,
			       const ref<implObj> &focus_to,
			       const ptr<elementObj::implObj> &focus_from,
			       focus_reporter_t focus_reporter)
		override;

	//! Override ensure_visibility.

	void ensure_visibility(IN_THREAD_ONLY, const rectangle &) override;

 protected:
	/////////////////////////////////////////////////////////////////////
	//
	// Inherited from horizvertObj

	void horizvert_updated(IN_THREAD_ONLY) override;

 public:

	//! Used by focusableImplObj to find the insert position in focusable_fields

	virtual focusable_fields_t::iterator
		initial_focusable_fields_insert_pos(IN_THREAD_ONLY);

	//! Received pasted text.

	//! Forwarded to the container, if not handled by the element.

	bool pasted(IN_THREAD_ONLY,
		    const std::experimental::u32string_view &str) override;
};

LIBCXXW_NAMESPACE_END

#endif
