/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_h
#define x_w_button_h

#include <x/w/buttonfwd.H>
#include <x/w/buttonobj.H>
#include <x/w/border_arg.H>
#include <x/w/element.H>
#include <x/w/focusable.H>
#include <x/w/border_arg.H>
#include <x/w/hotspot_bgcolor.H>
#include <x/w/text_param.H>
#include <x/w/shortcut.H>
#include <x/optional_args.H>

LIBCXXW_NAMESPACE_START

//! The visual appearance of a \ref button "button".

struct button_config {

	//! Default font for any text in the button
	font_arg button_font;

	//! Normal button background color
	color_arg normal_color;

	//! Background color when the button has input focus.
	color_arg selected_color;

	//! Background color when the button is activated
	color_arg active_color;

	//! Button's border.
	border_arg left_border;

	//! Button's border.
	border_arg right_border;

	//! Button's border.
	border_arg top_border;

	//! Button's border.
	border_arg bottom_border;

	//! Inner border when the button does not have input focus
	border_arg inputfocusoff_border;

	//! Inner border when the button has input focus
	border_arg inputfocuson_border;
};

//! Returns default appearance of a normal \ref button "button".
const button_config &normal_button();

//! Returns default appearance of a default \ref button "button".

//! Typically thicker border, to visually mark the default "Ok", or similar,
//! button, amongst its competitors.
const button_config &default_button();

LIBCXXW_NAMESPACE_END

#endif
