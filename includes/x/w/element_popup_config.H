/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_element_popup_config_h
#define x_w_element_popup_config_h

#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/scrollbar_appearance.H>
#include <x/w/generic_window_appearance.H>

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

	const_generic_window_appearance toplevel_appearance=
		generic_window_appearance::base::main_window_theme();

	//! Popup button image
	std::string button_image1="scroll-right1";

	//! Popup button image
	std::string button_image2="scroll-right2";

	//! Horizontal scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar=
		scrollbar_appearance::base::theme();

	//! Vertical scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar=
		scrollbar_appearance::base::theme();
};

LIBCXXW_NAMESPACE_END

#endif
