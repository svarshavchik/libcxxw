/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_impl_h
#define color_picker_impl_h

#include "color_picker/color_picker_handler.H"
#include "color_picker/color_picker_squarefwd.H"
#include "image_button_internalfwd.H"
#include "x/w/impl/popup/popupfwd.H"
#include "x/w/rgb.H"
#include "x/w/color_picker_configfwd.H"
#include "x/w/callback_triggerfwd.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN color_pickerObj::implObj : virtual public obj {

 public:

	//! The initial horizontal component shown by the color_picker_square
	static inline constexpr rgb_component_t rgb::*initial_horiz_component=
		&rgb::r;

	//! The initial vertical component shown by the color_picker_square
	static inline constexpr rgb_component_t rgb::*initial_vert_component=
		&rgb::g;

	//! The initial fixed component shown by the color_picker_square
	static inline constexpr rgb_component_t rgb::*initial_fixed_component=
		&rgb::b;

	//! The real internal implementation.

	const ref<handlerObj> handler;

	//! The popup button in the color picker.

	const image_button_internal popup_button;

	//! The popup itself.

	const popup color_picker_popup;

	//! Set this element's background color to the current color
	const element current_color_element;

	//! Show the horizontal component gradient color in this element.
	const element horiz_component_gradient;

	//! Show the vertical component gradient color in this element.
	const element vert_component_gradient;

	//! Show the fixed component gradient color in this element
	const element fixed_component_gradient;

	//! Show the two variable gradients here.
	const color_picker_square variable_gradients;

	//! Constructor
	implObj(const ref<handlerObj> &handler,
		const image_button_internal &popup_button,
		const popup &color_picker_popup,
		const element &current_color_element,

		const color_picker_config &config,
		const element &horiz_component_gradient,
		const element &vert_component_gradient,
		const element &fixed_component_gradient,
		const color_picker_square &variable_gradients);


	//! Destructor
	~implObj();

 private:

	//! Current horizontal component displayed in the color_picker_square
	rgb_component_t rgb::*horiz_component_thread_only=
		initial_horiz_component;

	//! Current vertical component displayed in the color_picker_square
	rgb_component_t rgb::*vert_component_thread_only=
		initial_vert_component;

	//! Current fixed component displayed in the color_picker_square
	rgb_component_t rgb::*fixed_component_thread_only=
		initial_fixed_component;

	//! Callback to invoke
	functionptr<color_picker_callback_t> callback_thread_only;

 public:
	THREAD_DATA_ONLY(horiz_component);
	THREAD_DATA_ONLY(vert_component);
	THREAD_DATA_ONLY(fixed_component);

	//! Most recently picked color

	//! Modified only in the connection thread, also fetched by
	//! current_color().

	mpobj<rgb> current_color;

	THREAD_DATA_ONLY(callback);

	//! Update the horizontal and vertical components

	void update_hv_components(ONLY IN_THREAD,
				  rgb_component_t v,
				  rgb_component_t h,
				  const callback_trigger_t &trigger);

	//! Update the fixed component

	void update_fixed_component(ONLY IN_THREAD,
				    rgb_component_t v,
				    const callback_trigger_t &trigger);

	//! Swap the horizontal component with the fixed component

	void swap_horiz_gradient(ONLY IN_THREAD);

	//! Swap the vertical component with the fixed component

	void swap_vert_gradient(ONLY IN_THREAD);

 private:

	//! Rebuild the horizontal, vertical, and fixed gradients.

	void refresh_component_gradients(ONLY IN_THREAD);

	//! Rebuild the square gradient

	void refresh_variable_gradients(ONLY IN_THREAD);

 public:
	//! Report the new color
	void new_color(ONLY IN_THREAD,
		       const callback_trigger_t &trigger);
};

LIBCXXW_NAMESPACE_END

#endif
