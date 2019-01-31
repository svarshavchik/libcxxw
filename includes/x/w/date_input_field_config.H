/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_config_h
#define x_w_date_input_field_config_h

#include <x/w/types.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/w/text_param.H>
#include <x/w/date_input_field_configfwd.H>

LIBCXXW_NAMESPACE_START

//! Custom visual appearance of a \ref date_input_field "date input field".

struct date_input_field_config {

	//! Border

	border_arg border="dateedit_border";

	//! Background color

	color_arg background_color="dateedit_background_color";

	//! Button focus border.
	border_arg focusoff_border="dateeditbuttonfocusoff_border";

	//! Button focus border.
	border_arg focuson_border="dateeditbuttonfocuson_border";

	//! Popup border
	border_arg popup_border="dateedit_popup_border";

	//! Popup background color
	color_arg popup_background_color="dateedit_popup_background_color";

	//! Error message to show for invalid input.

	text_param invalid_input=default_invalid_input();

	static text_param default_invalid_input() noexcept;

	//! Destructor

	~date_input_field_config();
};

LIBCXXW_NAMESPACE_END

#endif
