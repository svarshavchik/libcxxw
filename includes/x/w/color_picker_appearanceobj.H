/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_appearanceobj_h
#define x_w_color_picker_appearanceobj_h

#include <x/w/color_picker_appearancefwd.H>
#include <x/w/element_popup_appearancefwd.H>
#include <x/w/input_field_appearancefwd.H>
#include <x/w/button_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref color_picker_appearance "INSERT_LIBX_NAMESPACE::w::color_picker_appearance".

struct color_picker_appearance_properties {

	//! Constructor
	color_picker_appearance_properties();

	//! Destructor
	~color_picker_appearance_properties();

	//! Size of the current color swatch

	//! The size of the actual display element that shows the currently
	//! picked color.
	dim_arg width;

	//! Size of the current color swatch

	//! The size of the actual display element that shows the currently
	//! picked color.
	dim_arg height;

	//! Generic popup configuration
	const_element_popup_appearance attached_popup_appearance;

	//! The size of the top and bottom red/green/blue strips.
	dim_arg strip_width;

	//! The size of the left red/green/blue strip.
	dim_arg strip_height;

	//! Additional space between the component strips and the picker.
	dim_arg buffer_width;

	//! Additional space between the component strips and the picker.
	dim_arg buffer_height;

	//! Size of the picker gradient element.
	dim_arg picker_width;

	//! Size of the picker gradient element.
	dim_arg picker_height;

	//! Size of the buttons showing the basic colors
	dim_arg basic_color_width;

	//! Size of the buttons showing the basic colors
	dim_arg basic_color_height;

	//! Appearance of the H button
	const_button_appearance h_button_appearance;

	//! Appearance of the H button
	const_button_appearance v_button_appearance;

	//! Appearance of the basic colors buttons
	const_button_appearance basic_colors_button_appearance;

	//! Appearance of the R input field
	const_input_field_appearance r_appearance;

	//! Appearance of the G input field
	const_input_field_appearance g_appearance;

	//! Appearance of the B input field
	const_input_field_appearance b_appearance;

	//! Appearance of the H input field
	const_input_field_appearance h_appearance;

	//! Appearance of the S input field
	const_input_field_appearance s_appearance;

	//! Appearance of the V input field
	const_input_field_appearance v_appearance;
};

//! Implementation object.

//! \see color_picker_appearance

class color_picker_appearanceObj: public color_picker_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	color_picker_appearanceObj();

	//! Destructor
	~color_picker_appearanceObj();

	//! Copy constructor
	color_picker_appearanceObj(const color_picker_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_color_picker_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const color_picker_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_color_picker_appearance do_modify(const function
					<void(const color_picker_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
