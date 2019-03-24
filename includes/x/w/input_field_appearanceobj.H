/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_appearanceobj_h
#define x_w_input_field_appearanceobj_h

#include <x/w/input_field_appearancefwd.H>
#include <x/w/popup_list_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/button_config.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref input_field_appearance "INSERT_LIBX_NAMESPACE::w::input_field_appearance".

struct input_field_appearance_properties {

	//! Constructor
	input_field_appearance_properties();

	//! Destructor
	~input_field_appearance_properties();

	//! The input field's border.

	border_arg border;

	//! Inner border when the input field does not have input focus

	border_arg focusoff_border;

	//! Inner border when the input field has input focus

	border_arg focuson_border;

	//! Foreground color
	color_arg foreground_color;

	//! Font for normal input fields
	font_arg regular_font;

	//! Font for password input fields
	font_arg password_font;

	//! Background color
	color_arg background_color;

	//! Background color when the input field is disabled.

	//! Typical disabled element shading is not sufficient to distingish
	//! the visibility change.

	color_arg disabled_background_color;

	//! Hint's color
	color_arg hint_color;

	//! Free-play before dragging operation commences

	//! How far the pointer must move before drag operation starts.
	//! Avoids overhead when a pointer button click slips

	dim_arg drag_horiz_start;

	//! Free-play before dragging operation commences

	//! How far the pointer must move before drag operation starts.
	//! Avoids overhead when a pointer button click slips

	dim_arg drag_vert_start;

	//! Appearance of spinner buttons

	//! \note
	//! The spinner buttons' border comes from the border setting, and
	//! not here.
	button_config spinner_button_config;

	//! Appearance of the list search popup.

	const_popup_list_appearance search_popup_appearance;

	//! Horizontal scrollbar appearance
	const_scrollbar_appearance horizontal_scrollbar;

	//! Vertical scrollbar appearance
	const_scrollbar_appearance vertical_scrollbar;
};

//! Implementation object.

//! \see input_field_appearance

class input_field_appearanceObj: public input_field_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	input_field_appearanceObj();

	//! Destructor
	~input_field_appearanceObj();

	//! Copy constructor
	input_field_appearanceObj(const input_field_appearanceObj &);

	//! Create a copy of this object.
	input_field_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
