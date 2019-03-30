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
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref input_field_appearance "INSERT_LIBX_NAMESPACE::w::input_field_appearance".

struct input_field_appearance_properties {

	//! Constructor
	input_field_appearance_properties();

	//! Destructor
	~input_field_appearance_properties();

	//! Pointer image while typing, invisible pointer.
	std::string invisible_pointer;

	//! Pointer image while dragging
	std::string dragging_pointer;

	//! Pointer image while dragging non-droppable text
	std::string dragging_nodrop_pointer;

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
	//! The spinner buttons' borders should be based on our border.
	//! Use update_spinner_buttons() to install a new appearance object,
	//! that comes from the default button appearance them, with our
	//! borders.

	button_config left_spinner_button_config;

	//! Appearance of spinner buttons

	//! \note
	//! The spinner buttons' borders should be based on our border.
	//! Use update_spinner_buttons() to install a new appearance object,
	//! that comes from the default button appearance them, with our
	//! borders.
	button_config right_spinner_button_config;

	//! Update spinner button appearances.
	void update_spinner_buttons();

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
	template<typename F> inline const_input_field_appearance modify(F &&f)
		const
	{
		return do_modify(make_function
				 <void(const input_field_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_input_field_appearance
	do_modify(const function<void(const input_field_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
