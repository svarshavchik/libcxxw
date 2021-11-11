/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_impl_h
#define color_picker_impl_h

#include "color_picker/color_picker_squarefwd.H"
#include "color_picker/color_picker_alpha_canvasfwd.H"
#include "color_picker/color_picker_current_canvasfwd.H"
#include "popup/popup_attachedto_elementfwd.H"
#include "image_button_internalfwd.H"
#include "x/w/impl/popup/popupfwd.H"
#include "x/w/rgb.H"
#include "x/w/color_picker.H"
#include "x/w/color_picker_configfwd.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/input_fieldfwd.H"
#include "x/w/labelfwd.H"
#include "x/w/validated_input_field.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START


//! Internal implementation object.

class LIBCXX_HIDDEN color_pickerObj::implObj : virtual public obj {

 public:

	//! The color picker's official color.

	//! The official color changes only after choosing a color and
	//! closing the popup in the affirmative.
	//!
	//! Stored in a separate object. The official color is owned by
	//! this implementation object, and also referenced by a display
	//! element that save()s it, to be restore()d later.

	class official_colorObj : virtual public obj {

	public:
		//! Official color

		//! Pressing "Ok" moves current_color to official_color

		mpobj<rgb> official_color;

		official_colorObj(const rgb &initial_color)
			: official_color{initial_color}
		{
		}
	};

	//! Alias
	typedef ref<official_colorObj> official_color;

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

	const popup_attachedto_element_impl impl;

	//! The popup button in the color picker.

	const image_button_internal popup_button;

	//! Set this element's background color to the current color
	const color_picker_current_canvas current_color_element;

	//! Show the horizontal component gradient color in this element.
	const element horiz_component_gradient;

	//! Show the vertical component gradient color in this element.
	const element vert_component_gradient;

	//! Alpha component gradient
	const color_picker_alpha_canvas alpha_component_gradient;

	//! Show the fixed component gradient color in this element
	const element fixed_component_gradient;

	//! Show the two variable gradients here.
	const color_picker_square variable_gradients;

	//! The error message field
	const label error_message_field;

	//! Constructor
	implObj(const popup_attachedto_element_impl &handler,
		const image_button_internal &popup_button,
		const color_picker_current_canvas &current_color_element,

		const color_picker_config &config,
		const official_color &initial_color,
		const element &horiz_component_gradient,
		const element &vert_component_gradient,
		const color_picker_alpha_canvas &alpha_component_gradient,
		const element &fixed_component_gradient,
		const color_picker_square &variable_gradients,

		const label &error_message_field);


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

	//! Whether to display manual values in hexadecimal

	mpobj<bool> hexadecimal=false;

	//! Whether to display values in full precision

	mpobj<bool> full_precision=false;

 private:
	//! Most recently picked color

	//! Modified only in the connection thread, also fetched by
	//! current_color().

	rgb current_color_thread_only;
 public:

	THREAD_DATA_ONLY(current_color);

	const official_color current_official_color;

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

	//! Update the alpha component

	void update_alpha_component(ONLY IN_THREAD,
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
	//! Set the current color to a particular value
	void set_color(ONLY IN_THREAD, const rgb &c);
	//! Report the new color
	void new_color(ONLY IN_THREAD,
		       const callback_trigger_t &trigger);

	//! A new value was entered for R, G, or B.
	void new_rgb_values(ONLY IN_THREAD);

	//! A new value was entered for A
	void new_alpha_value(ONLY IN_THREAD);

 private:
	//! A new value was entered for R, G, B, or A.

	//! Entering a new alpha values does not update the HSV values.
	void new_rgba_values(ONLY IN_THREAD, bool update_hsv);

 public:
	//! A new value was entered for H, S, or V.
	void new_hsv_values(ONLY IN_THREAD);

	//! Compute HSV from RGB.

	//! Used during construction to initialize HSV fields.

	static std::tuple<rgb_component_t, rgb_component_t, rgb_component_t>
		compute_hsv(const rgb &color);

	//! The manually-entered red component
	validated_input_fieldptr<rgb_component_t> r_value;

	//! The manually-entered green component
	validated_input_fieldptr<rgb_component_t> g_value;

	//! The manually-entered blue component
	validated_input_fieldptr<rgb_component_t> b_value;

	//! The manually-entered alpha component
	validated_input_fieldptr<rgb_component_t> a_value;

	//! The manually-entered hue component
	validated_input_fieldptr<rgb_component_t> h_value;

	//! The manually-entered saturation component
	validated_input_fieldptr<rgb_component_t> s_value;

	//! The manually-entered value component
	validated_input_fieldptr<rgb_component_t> v_value;

	//! Reformat the fields after full_precision or hexadecimal settings are changed.

	void reformat_values(ONLY IN_THREAD);

	//! The "Ok" button in the popup was pressed.

	//! current_color becomes official_color, and we announce that
	//! official_color_updated().
	void set_official_color(ONLY IN_THREAD);

	//! The popup has been closed

	//! Restore the official_color, if it's different.
	void popup_closed(ONLY IN_THREAD);

	//! Invoke the callback, if one is installed.
	void official_color_updated(ONLY IN_THREAD,
				    const callback_trigger_t &t);
};

LIBCXXW_NAMESPACE_END

#endif
