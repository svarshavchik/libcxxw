/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_appearanceobj_h
#define x_w_date_input_field_appearanceobj_h

#include <x/w/date_input_field_appearancefwd.H>
#include <x/w/input_field_appearancefwd.H>
#include <x/w/image_button_appearancefwd.H>
#include <x/w/generic_window_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref date_input_field_appearance "INSERT_LIBX_NAMESPACE::w::date_input_field_appearance".

struct date_input_field_appearance_properties {

	//! Constructor
	date_input_field_appearance_properties();

	//! Destructor
	~date_input_field_appearance_properties();

	//! Button image
	std::string popup_button_image1;

	//! Pressed button image
	std::string popup_button_image2;

	//! Foreground color: day of month highlighted by pointer or keyboard.
	text_color_arg day_highlight_fg;

	//! Background color: day of month highlighted by pointer or keyboard.

	text_color_arg day_highlight_bg;

	//! Font for the current month's name (and year)
	font_arg month_font;

	//! Font color for the current month's name (and year)
	text_color_arg month_color;

	//! Font for the days of week column titles.

	font_arg day_of_week_font;

	//! Font color for the days of week column titles.

	text_color_arg day_of_week_font_color;

	//! Appearance of the button on the date popup.
	const_image_button_appearance previous_year_appearance;

	//! Appearance of the button on the date popup.
	const_image_button_appearance previous_month_appearance;

	//! Appearance of the button on the date popup.
	const_image_button_appearance next_month_appearance;

	//! Appearance of the button on the date popup.
	const_image_button_appearance next_year_appearance;

	//! Height of buttons that advance the calendar by a year
	dim_arg yscroll_height;

	//! Height of buttons that advance the calendar by a month
	dim_arg mscroll_height;

	//! Appearance of the input field element itself

	//! The default input field appearance, except there's no border
	//! (the date input field provides it, and the date input field
	//! has its own background color.

	const_input_field_appearance input_appearance;

	//! Input field's font.
	font_arg input_field_font;

	//! Input field's font's color
	text_color_arg input_field_font_color;

	//! Border

	border_arg border;

	//! Button focus border.
	border_arg focusoff_border;

	//! Button focus border.
	border_arg focuson_border;

	//! Popup border
	border_arg popup_border;

	//! Popup background color
	color_arg popup_background_color;

	//! Generic top level window appearance properties
	const_generic_window_appearance toplevel_appearance;
};

//! Implementation object.

//! \see date_input_field_appearance

class date_input_field_appearanceObj
	: public date_input_field_appearance_properties,
	  virtual public obj {

public:
	//! Constructor
	date_input_field_appearanceObj();

	//! Destructor
	~date_input_field_appearanceObj();

	//! Copy constructor
	date_input_field_appearanceObj(const date_input_field_appearanceObj &);

	//! Create a copy of this object.

	template<typename F>
	inline const_date_input_field_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const date_input_field_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_date_input_field_appearance
	do_modify(const function<void(const date_input_field_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif