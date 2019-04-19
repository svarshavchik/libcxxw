/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_impl_h
#define scrollbar_impl_h

#include "x/w/impl/focus/focusable_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/always_visible_elementfwd.H"
#include "scrollbar/scrollbar_icon_set.H"
#include "scrollbar/scrollbar_metrics.H"
#include "icon_images_set_elementfwd.H"
#include "themedim_element_minoverridefwd.H"
#include "x/w/scrollbar.H"
#include "x/w/dim_arg.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

struct scrollbar_orientation;

extern const scrollbar_orientation horizontal_scrollbar LIBCXX_HIDDEN;

extern const scrollbar_orientation vertical_scrollbar LIBCXX_HIDDEN;

//! Create a horizontal scrollbar.

//! To do that, we need the scrollbar's parent container,
//! a \ref scrollbar_config "scrollbar_config", and the scrollbar's
//! initial callback.
//!
//! This only creates the scrollbar element linked to the parent_container.
//! It is the caller's responsibility to, at some point, add it to the
//! parent using created_internally().

scrollbar do_create_h_scrollbar(const container_impl
				&parent_container,
				const std::optional<color_arg> &background_color,
				const scrollbar_config &conf,
				const dim_arg &minimum_size,
				const scrollbar_cb_t &callback)
	LIBCXX_HIDDEN;

//! Create a vertical scrollbar.

//! Like create_horizontal_scrollbar(), but makes a vertical one.

scrollbar do_create_v_scrollbar(const container_impl
				&parent_container,
				const std::optional<color_arg> &background_color,
				const scrollbar_config &conf,
				const dim_arg &minimum_size,
				const scrollbar_cb_t &callback)
	LIBCXX_HIDDEN;

//! Parameters for scrollbarObj::implObj's constructor

//! An external factory actually create()s the implementation object, and it
//! shouldn't care about the parameters, so we encapsulate them here.

struct LIBCXX_HIDDEN scrollbar_impl_init_params {
	const container_impl &container;
	const scrollbar_cb_t &callback;
	const scrollbar_orientation &orientation;
	const scrollbar_icons_tuple_t &icons;
	const scrollbar_config &conf;
	const dim_arg &minimum_size;
};

//! Scrollbar implementation object.

class LIBCXX_HIDDEN scrollbarObj::implObj :
	public themedim_element_minoverrideObj<
	scrollbar_icon_images_set_t<always_visible_elementObj
				    <focusable_elementObj<>>>> {

	//! Normal or pressed icons.

	struct LIBCXX_HIDDEN scrollbar_icon_set {
		icon low;
		icon high;
		icon handlebar_start;
		icon handlebar;
		icon handlebar_end;
	};

	//! Return the normal icons.

	scrollbar_icon_set normal_icons(ONLY IN_THREAD);

	//! Return the pressed icons.

	scrollbar_icon_set pressed_icons(ONLY IN_THREAD);

	//! My superclass

	typedef themedim_element_minoverrideObj<
		scrollbar_icon_images_set_t<always_visible_elementObj<
			focusable_elementObj<>>>> superclass_t;

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

	dim_t current_position_major_size(ONLY IN_THREAD) const;

	//! Return the current minor size.

	//! For a horizontal scrollbar returns current_position.height.
	//! For a vertical scrollbar returns current_position.width.

	dim_t current_position_minor_size(ONLY IN_THREAD) const;

	//! Current, internal scrollbar configuration
	scrollbar_config state_thread_only;

	//! Current callback
	scrollbar_cb_t updated_value_thread_only;

	//! Current scrollbar drag value

	scroll_v_t dragged_value;

	//! This is normally same as conf.value, except when dragging.

	//! Sanity-check the values in conf().
	void validate_conf(ONLY IN_THREAD);

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

	void reset_state(ONLY IN_THREAD);

	//! Pixel value when dragging started.
	scroll_v_t drag_start_current_pixel=0;

	//! The current major size of the scroll_low button.
	dim_t scroll_low_size(ONLY IN_THREAD) const;

	//! The current major size of the scroll_high button.
	dim_t scroll_high_size(ONLY IN_THREAD) const;

	//! The scroll_high button's drawing position.
	coord_t scroll_high_position(ONLY IN_THREAD) const;

	//! The slider area's size.
	dim_t slider_size(ONLY IN_THREAD) const;

	//! The current major size of the handlebar start image.
	dim_t handlebar_start_size(ONLY IN_THREAD) const;

	//! The current major size of the handlebar image.
	dim_t handlebar_size(ONLY IN_THREAD) const;

	//! The current major size of the handlebar end image.
	dim_t handlebar_end_size(ONLY IN_THREAD) const;

 public:
	THREAD_DATA_ONLY(state);
	THREAD_DATA_ONLY(updated_value);

	//! Public scrollbar status

	//! status gets updated from "state", which is considered to be
	//! internal, and is accessible only by the connection thread.

	mpobj<scrollbar_config> status;

	//! Advance slider towards 0.

	//! Returns true if the slider was moved and redrawn.
	bool to_low(ONLY IN_THREAD, const input_mask &mask);

	//! Advance slider towards its maximum value.

	//! Returns true if the slider was moved and redrawn.
	bool to_high(ONLY IN_THREAD, const input_mask &mask);

	//! Advance slider towards 0, by page size.

	//! Returns true if the slider was moved and redrawn.
	bool page_up(ONLY IN_THREAD);

	//! Advance slider towards its maximum value, by page size.

	//! Returns true if the slider was moved and redrawn.
	bool page_down(ONLY IN_THREAD);

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
	void initialize(ONLY IN_THREAD) override;

	//! Theme has been updated.

	//! theme_updated() all icons. Invoke recalculate_metrics().

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme) override;

	//! Scrollbar's size has been updated.

	//! Invoke calculate_scrollbar_metrics().
	void current_position_updated(ONLY IN_THREAD) override;

	//! Redraw the scrollbar.

	//! Invokes do_draw_scroll_low() and do_draw_scroll_hi()
	//! (unless the display element is too small for the scrollbar.
	//!
	//! Invokes do_draw_slider() if the display element is big enough
	//! for it.
	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;

	//! Process key events.
	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	//! Process button events.
	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Process motion events.

	void report_motion_event(ONLY IN_THREAD, const motion_event &)
		override;

	//! Process a focus change notification.

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	//////////////////////////////////////////////////////////////////

	//! Manually update the scrollbar's configuration.

	//! This updates the scrollbar's configuration. The updated_value()
	//! callback does not get invoked.

	void reconfigure(ONLY IN_THREAD,
			 const scrollbar_config &new_config);

	//! Update the callback

	void update_callback(ONLY IN_THREAD,
			     const scrollbar_cb_t &);
 private:
	//! Release the handlebar, if it's pressed.

	//! Called when processing a key event, to make sure the handlebar
	//! state is released, if somehow a key event is received in the
	//! middle of dragging.
	void abort_handlebar(ONLY IN_THREAD);

	//! Clean up after an aborted drag.

	//! Called when the button is released outside of the scrollbar,
	//! or when the input element loses focus in the middle of the drag.

	void abort_dragging(ONLY IN_THREAD);

	//! A value has been updated, no dragging took place.

	//! Sets drag_value to conf.value, calls draw_slider() and
	//! updated_value().

	void updated_value_no_drag(ONLY IN_THREAD);

	//! Invoke the callback if the values are different than previous values

	void report_changed_values(ONLY IN_THREAD,
				   scroll_v_t value,
				   scroll_v_t dragged_value);

	//! Invoke the callback

	void report_current_values(ONLY IN_THREAD,
				   scroll_v_t value,
				   scroll_v_t dragged_value,
				   const callback_trigger_t &why);
 public:
	//! Mutex-protected container for the current value of the scrollbar

	typedef mpobj<std::tuple<scroll_v_t, scroll_v_t>> current_value_t;

	//! report_updated_value() saves its values here.

	current_value_t current_value;

 private:
	//! Compute and set the scrollbar display element's metrics.

	//! The height of the horizontal scrollbar and the width of the
	//! vertical scrollbar is based on the icons' corresponding size.
	//!
	//! Use this to set our element metrics. This is done when this
	//! display element gets initialize()d, or when theme_updated(),
	//! because a new theme means new icons.
	void recalculate_metrics(ONLY IN_THREAD);

	//! Compute metrics based on the current size of the scrollbar.

	//! This adapts the scrollbar based on the current display element's
	//! size, computing the dimensions of the slider, and resizing the
	//! icons accordingly.
	//!
	//! Returns false if the scrollbar metrics have not actually changed.
	bool calculate_scrollbar_metrics(ONLY IN_THREAD);

	//! What the scrollbar metrics were based on
	dim_t metrics_based_on_width=0;

	//! What the scrollbar metrics were based on
	dim_t metrics_based_on_height=0;

	//! Draw the 'scroll low' button

	//! Schedules the scroll low button to be redrawn.

	void draw_scroll_low(ONLY IN_THREAD);

	//! Draw the "scroll low" button into the given scratch buffer.

	void do_draw_scroll_low(ONLY IN_THREAD, const picture &buffer);

	//! Draw the 'scroll high' button

	//! Schedules the scroll high button to be redrawn.

	void draw_scroll_high(ONLY IN_THREAD);

	//! Draw the "scroll high" button.

	void do_draw_scroll_high(ONLY IN_THREAD, const picture &buffer);

	//! Draw the slider.

	//! Schedules the slider to be redrawn.

	void draw_slider(ONLY IN_THREAD);

	//! Draw the scrollbar slider

	void do_draw_slider(ONLY IN_THREAD, const picture &buffer);

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

	coord_t do_draw_icon(ONLY IN_THREAD,
			     const picture &buffer,
			     icon scrollbar_icon_set::*which_icon,
			     bool pressed,
			     coord_t major_coord);

	//! Compute where the low scroll, slider, and high scroll get drawn

	//! If the scrollbar is too small, each one is the entire scrollbar.

	std::tuple<rectangle, rectangle,
		   rectangle> regions(ONLY IN_THREAD) const;
};

LIBCXXW_NAMESPACE_END

#endif
