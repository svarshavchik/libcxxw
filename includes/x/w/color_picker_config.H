/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_config_h
#define x_w_color_picker_config_h

#include <x/w/color_picker_configfwd.H>
#include <x/w/color_pickerfwd.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

struct color_picker_config {

	//! Initial color
	rgb initial_color=rgb{rgb::maximum, rgb::maximum, rgb::maximum};

	//! Initial callback
	functionptr<color_picker_callback_t> initial_callback;

	//! Border

	border_arg border="color_picker_border";

	//! Background color

	color_arg background_color="color_picker_background_color";

	//! Popup border
	border_arg popup_border="color_picker_popup_border";

	//! Popup background color
	color_arg popup_background_color="color_picker_popup_background_color";

	//! Button focus border.
	border_arg focusoff_border="color_pickerbuttonfocusoff_border";

	//! Button focus border.
	border_arg focuson_border="color_pickerbuttonfocuson_border";
};

LIBCXXW_NAMESPACE_END

#endif
