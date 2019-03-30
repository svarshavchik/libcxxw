/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_config_h
#define x_w_color_picker_config_h

#include <x/w/color_picker_configfwd.H>
#include <x/w/color_pickerfwd.H>
#include <x/w/element_popup_appearance.H>
#include <x/w/dim_arg.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Initial color picker configuration

//! An optional parameter to a \ref factory "factory"'s
//! create_color_picker() setting the initial value and apperance of a
//! \ref color_picker "INSERT_LIBX_NAMESPACE::w::color_picker".

struct color_picker_config {

	//! Initial callback
	functionptr<color_picker_callback_t> initial_callback;

	//! Initial color
	rgb initial_color=rgb{rgb::maximum, rgb::maximum, rgb::maximum};

	//! Size of the current color swatch

	//! The size of the actual display element that shows the currently
	//! picked color.
	dim_arg width="color_picker_current_width";

	//! Size of the current color swatch

	//! The size of the actual display element that shows the currently
	//! picked color.
	dim_arg height="color_picker_current_height";

	//! Generic popup configuration
	const_element_popup_appearance attached_popup_appearance=
		const_element_popup_appearance::base::theme();

	//! The size of the top and bottom red/green/blue strips.
	dim_arg strip_width="color_picker_strip_width";

	//! The size of the left red/green/blue strip.
	dim_arg strip_height="color_picker_strip_height";

	//! Additional space between the component strips and the picker.
	dim_arg buffer_width="color_picker_buffer_width";

	//! Additional space between the component strips and the picker.
	dim_arg buffer_height="color_picker_buffer_height";

	//! Size of the picker gradient element.
	dim_arg picker_width="color_picker_square_width";

	//! Size of the picker gradient element.
	dim_arg picker_height="color_picker_square_height";

	//! Size of the buttons showing the basic colors
	dim_arg basic_color_width="color_picker_basic_color_width";

	//! Size of the buttons showing the basic colors
	dim_arg basic_color_height="color_picker_basic_color_height";

};

LIBCXXW_NAMESPACE_END

#endif
