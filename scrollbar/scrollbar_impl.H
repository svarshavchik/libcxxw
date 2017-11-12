/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_impl_h
#define scrollbar_impl_h

#include "focus/focusable_elementfwd.H"
#include "child_element.H"
#include "scrollbar/scrollbar.H"
#include "scrollbar/scrollbar_icon_set.H"
#include "scrollbar/scrollbar_metrics.H"
#include "icon_images_set_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Parameters for scrollbarObj::implObj's constructor

//! An external factory actually create()s the implementation object, and it
//! shouldn't care about the parameters, so we encapsulate them here.

struct LIBCXX_HIDDEN scrollbar_impl_init_params {
	const ref<containerObj::implObj> &container;
	const scrollbar_orientation &orientation;
	const scrollbar_icons_tuple_t &icons;
	const scrollbar_config &conf;
};

//! Scrollbar implementation object.

class LIBCXX_HIDDEN scrollbarObj::implObj :
	public scrollbar_icon_images_set_t<focusable_elementObj<>> {

	//! Normal or pressed icons.

	struct LIBCXX_HIDDEN scrollbar_icon_set {
		icon low;
		icon high;
		icon handlebar_start;
		icon handlebar;
		icon handlebar_end;
	};

	//! Return the normal icons.

	scrollbar_icon_set normal_icons(IN_THREAD_ONLY);

	//! Return the pressed icons.

	scrollbar_icon_set pressed_icons(IN_THREAD_ONLY);

	//! My superclass

	typedef scrollbar_icon_images_set_t<focusable_elementObj<>
					    > superclass_t;

	//! Specifies whether this is a horizontal or a vertical scrollbar

	const scrollbar_orientation &orientation;

	//! Return the major dimension

	//! For the horizontal scrollbar returns the width.
	//! For the vertical scrollbar returns the height.

	dim_t major_size(const rectangle &) const;

	//! Return the minor dimension

	//! For the horizontal scrollbar returns the height.
	//! For the vertical scrollbar returns the width.

	dim_t minor_size(const rectangle &) const;

	//! Return the icon's dimensions as a rectangle.

	static rectangle icon_size(const icon &);

	//! Return an icon's major size.

	//! For a horizontal scrollbar returns the icon's width.
	//! For a vertical scrollbar returns the icon's height.

	dim_t major_icon_size(const icon &) const;

	//! Return an icon's minor size.

	//! For a horizontal scrollbar returns the icon's height.
	//! For a vertical scrollbar returns the icon's width.

	dim_t minor_icon_size(const icon &) const;

	//! Return the current major size.

	//! For a horizontal scrollbar returns current_position.width.
	//! For a vertical scrollbar returns current_position.height.

	dim_t current_position_major_size(IN_THREAD_ONLY) const;

	//! Return the current minor size.

	//! For a horizontal scrollbar returns current_position.height.
	//! For a vertical scrollbar returns current_position.width.

	dim_t current_position_minor_size(IN_THREAD_ONLY) const;

	//! Current scrollbar configuration
	scrollbar_config conf;

	//! Current scrollbar drag value

	scroll_v_t dragged_value;

	//! This is normally same as conf.value, except when dragging.

	//! Sanity-check the values in conf().
	void validate_conf();

	//! Current metrics for the parts of the scrollbar.
	scrollbar_metrics metrics;

	//! Current pixel position of the handlebar
	scroll_v_t current_pixel=0;

	//! Whether scroll_low should be drawn as pressed
	bool scroll_low_pressed=false;

	//! Whether the handlebra should be drawn as pressed
	bool handlebar_pressed=false;

	//! Whether scroll_low should be drawn as pressed
	bool scroll_high_pressed=false;

	//! Dragging has started
	bool dragging=false;

	//! Starting point while dragging.
	coord_t drag_start=0;

	//! Reset the state to default.

	//! Something drastic has happened. Like the scrollbar's size has
	//! change, or the scrollbar has lost focus.
	//!
	//! If we were in a middle of something, like a button or a key press,
	//! none of that applies. Reset all the flags to defaults.

	void reset_state(IN_THREAD_ONLY);

	//! Pixel value when dragging started.
	scroll_v_t drag_start_current_pixel=0;

	//! The current major size of the scroll_low button.
	dim_t scroll_low_size(IN_THREAD_ONLY) const;

	//! The current major size of the scroll_high button.
	dim_t scroll_high_size(IN_THREAD_ONLY) const;

	//! The scroll_high button's drawing position.
	coord_t scroll_high_position(IN_THREAD_ONLY) const;

	//! The slider area's size.
	dim_t slider_size(IN_THREAD_ONLY) const;

	//! The current major size of the handlebar start image.
	dim_t handlebar_start_size(IN_THREAD_ONLY) const;

	//! The current major size of the handlebar image.
	dim_t handlebar_size(IN_THREAD_ONLY) const;

	//! The current major size of the handlebar end image.
	dim_t handlebar_end_size(IN_THREAD_ONLY) const;

 public:
	//! Advance slider towards 0.

	//! Returns true if the slider was moved and redrawn.
	bool to_low(IN_THREAD_ONLY, const input_mask &mask);

	//! Advance slider towards its maximum value.

	//! Returns true if the slider was moved and redrawn.
	bool to_high(IN_THREAD_ONLY, const input_mask &mask);

	//! Advance slider towards 0, by page size.

	//! Returns true if the slider was moved and redrawn.
	bool page_up(IN_THREAD_ONLY);

	//! Advance slider towards its maximum value, by page size.

	//! Returns true if the slider was moved and redrawn.
	bool page_down(IN_THREAD_ONLY);

	//! Internal constructor

	implObj(const scrollbar_impl_init_params &init_params);

	//! Destructor
	~implObj();

	////////////////////////////////////////////////////////////////////
	//
	// Inherited

	//! Initialize after creation.

	//! initialize() all icons. Invoke recalculate_metrics() and
	//! calculate_scrollbar_metrics().
	void initialize(IN_THREAD_ONLY) override;

	//! Theme has been updated.

	//! theme_updated() all icons. Invoke recalculate_metrics().

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;

	//! Scrollbar's size has been updated.

	//! Invoke calculate_scrollbar_metrics().
	void current_position_updated(IN_THREAD_ONLY) override;

	//! Redraw the scrollbar.

	//! Invokes do_draw_scroll_low() and do_draw_scroll_hi()
	//! (unless the display element is too small for the scrollbar.
	//!
	//! Invokes do_draw_slider() if the display element is big enough
	//! for it.
	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas);

	//! Process key events.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! Process button events.
	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp);

	//! Process motion events.

	void report_motion_event(IN_THREAD_ONLY, const motion_event &)
		override;

	//! Process a focus change notification.

	void keyboard_focus(IN_THREAD_ONLY) override;

	//////////////////////////////////////////////////////////////////

	//! The value of the scrollbar has changed.

	//! When dragging the scrollbar by its handle, the official value
	//! of the scrollbar changes only when the scrollbar handle gets
	//! released. \c drag_value reflects the value of the scrollbar
	//! as its being dragged, while \c value remains unchanged.
	//!
	//! The default implementation does nothing, overridden in a subclass.
	virtual void updated_value(IN_THREAD_ONLY,
				   scroll_v_t value,
				   scroll_v_t dragged_value)=0;

	//! Manually update the scrollbar's configuration.

	//! This updates the scrollbar's configuration. The updated_value()
	//! callback does not get invoked.

	void update_config(IN_THREAD_ONLY,
			   const scrollbar_config &new_config);
 private:
	//! Release the handlebar, if it's pressed.

	//! Called when processing a key event, to make sure the handlebar
	//! state is released, if somehow a key event is received in the
	//! middle of dragging.
	void abort_handlebar(IN_THREAD_ONLY);

	//! Clean up after an aborted drag.

	//! Called when the button is released outside of the scrollbar,
	//! or when the input element loses focus in the middle of the drag.

	void abort_dragging(IN_THREAD_ONLY);

	//! A value has been updated, no dragging took place.

	//! Sets drag_value to conf.value, calls draw_slider() and
	//! updated_value().

	void updated_value_no_drag(IN_THREAD_ONLY);

	//! Compute and set the scrollbar display element's metrics.

	//! The height of the horizontal scrollbar and the width of the
	//! vertical scrollbar is based on the icons' corresponding size.
	//!
	//! Use this to set our element metrics. This is done when this
	//! display element gets initialize()d, or when theme_updated(),
	//! because a new theme means new icons.
	void recalculate_metrics(IN_THREAD_ONLY);

	//! Compute metrics based on the current size of the scrollbar.

	//! This adapts the scrollbar based on the current display element's
	//! size, computing the dimensions of the slider, and resizing the
	//! icons accordingly.
	//!
	//! Returns false if the scrollbar metrics have not actually changed.
	bool calculate_scrollbar_metrics(IN_THREAD_ONLY);

	//! What the scrollbar metrics were based on
	dim_t metrics_based_on_width=0;

	//! What the scrollbar metrics were based on
	dim_t metrics_based_on_height=0;

	//! Draw the 'scroll low' button

	//! Constructs a scratch buffer and invokes do_draw_scroll_low().

	void draw_scroll_low(IN_THREAD_ONLY);

	//! Draw the "scroll low" button into the given scratch buffer.

	void do_draw_scroll_low(IN_THREAD_ONLY, const picture &buffer);

	//! Draw the 'scroll high' button

	//! Constructs a scratch buffer and invokes do_draw_scroll_high().

	void draw_scroll_high(IN_THREAD_ONLY);

	//! Draw the "scroll high" button.

	//! To redraw the button only, this is called with a scratch
	//! buffer for just the button, and the coordinate of 0.
	//!
	//! When redrawing the entire display element we use a scratch buffer
	//! for the entire display element, and pass the scroll_high_position()
	//! for the coordinate.

	void do_draw_scroll_high(IN_THREAD_ONLY, const picture &buffer,
				 coord_t coordinate);

	//! Draw the slider.

	//! Constructs a scratch buffer and invokes do_draw_slider().

	void draw_slider(IN_THREAD_ONLY);

	//! Draw the scrollbar slider

	//! To redraw the slider only, this is called with a scratch
	//! buffer for just the button, and the coordinate of 0.
	//!
	//! When redrawing the entire display element we use a scratch buffer
	//! for the entire display element, and pass scroll_low_size() for the
	//! coordinate.
	void do_draw_slider(IN_THREAD_ONLY, const picture &buffer,
			    coord_t coordinate);

	//! Draw an icon at the given major coordinate

	//! Used by do_draw_scroll_low(), do_draw_scoll_high() and
	//! do_draw_slider().
	//!
	//! which_icon specifies the icon.
	//!
	//! pressed selects either icon_set_1 or icon_set_2.
	//!
	//! major_coord specifies the horizontal coordinate in the buffer
	//! to draw into, for horizontal sliders; and the vertical coordinate
	//! for vertical sliders.
	//!
	//! Returns major_coord plus either the icon's width, for horizontal
	//! slider, or the icon's height for vertical sliders.

	coord_t do_draw_icon(IN_THREAD_ONLY,
			     const picture &buffer,
			     icon scrollbar_icon_set::*which_icon,
			     bool pressed,
			     coord_t major_coord);
};

LIBCXXW_NAMESPACE_END

#endif
