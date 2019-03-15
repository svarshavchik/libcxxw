/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_config_h
#define x_w_date_input_field_config_h

#include <x/w/types.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/text_param.H>
#include <x/w/date_input_field_configfwd.H>
#include <x/w/input_field_config.H>

LIBCXXW_NAMESPACE_START

struct date_input_field_popup_config {

};

//! Custom visual appearance of a \ref date_input_field "date input field".

struct date_input_field_config : public date_input_field_popup_config {

	//! Appearance of the input field element itself

	//! The default input field appearance, except there's no bder
	//! (the date input field provides it, and the date input field
	//! has its own background color.

	input_field_config_appearance input_field_appearance
		{
		 .border={},
		 .background_color="dateedit_background_color",
		};

	//! Input field's font.
	font_arg input_field_font=theme_font{"dateedit"};

	//! Input field's font's color
	text_color_arg input_field_font_color=theme_color
		{
		 "dateedit_foreground_color"
		};

	//! Border

	border_arg border="dateedit_border";

	//! Button focus border.
	border_arg focusoff_border="dateeditbuttonfocusoff_border";

	//! Button focus border.
	border_arg focuson_border="dateeditbuttonfocuson_border";

	//! Popup border
	border_arg popup_border="dateedit_popup_border";

	//! Popup background color
	color_arg popup_background_color="dateedit_popup_background_color";

	//! Font used in the main date input field

	//! Font for the calendar popup.
	font_arg popup_font=theme_font{"dateedit_day"};

	//! Label color for the calendar popup.
	color_arg popup_font_color="dateedit_day";

	//! Error message to show for invalid input.

	text_param invalid_input=default_invalid_input();

	static text_param default_invalid_input() noexcept;

	//! Destructor

	~date_input_field_config();
};

LIBCXXW_NAMESPACE_END

#endif
