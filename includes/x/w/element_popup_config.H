/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_popup_config_h
#define x_w_element_popup_config_h

#include <x/w/border_arg.H>
#include <x/w/rgb.H>

LIBCXXW_NAMESPACE_START

//! Common configuration settings for elements with attached popups.

struct element_popup_config {

	//! Border around the display element
	border_arg element_border="element_attached_border";

	//! Popup border
	border_arg popup_border="element_attached_popup_border";

	//! Popup background color
	color_arg popup_background_color="element_attached_popup_background_color";

	//! Popup background color
	color_arg popup_scrollbars_background_color="element_attached_popup_background_color";

	//! Button focus border.
	border_arg button_focusoff_border="element_attached_popup_button_focusoff_border";

	//! Button focus border.
	border_arg button_focuson_border="element_attached_popup_button_focuson_border";

	//! Popup button image
	std::string button_image1="scroll-right1";

	//! Popup button image
	std::string button_image2="scroll-right2";
};

LIBCXXW_NAMESPACE_END

#endif
