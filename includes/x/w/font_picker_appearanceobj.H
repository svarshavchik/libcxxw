/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_appearanceobj_h
#define x_w_font_picker_appearanceobj_h

#include <x/w/font_picker_appearancefwd.H>
#include <x/w/scrollbar_appearancefwd.H>
#include <x/w/combobox_appearancefwd.H>
#include <x/w/element_popup_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/text_param.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref font_picker_appearance "INSERT_LIBX_NAMESPACE::w::font_picker_appearance".

struct font_picker_appearance_properties {

	//! Constructor
	font_picker_appearance_properties();

	//! Destructor
	~font_picker_appearance_properties();

	//! Label for the "Ok" button
	text_param ok_label;

	//! Label for the "Cancel" button
	text_param cancel_label;

	//! Size of the font preview box
	dim_arg preview_width;

	//! Size of the font preview box
	dim_arg preview_height;

	//! Border around the font preview box
	border_arg preview_border;

	//! Combo-box appearance
	const_combobox_appearance font_family_appearance;

	//! Combo-box appearance
	const_combobox_appearance font_size_appearance;

	//! Combo-box appearance
	const_combobox_appearance font_weight_appearance;

	//! Combo-box appearance
	const_combobox_appearance font_slant_appearance;

	//! Combo-box appearance
	const_combobox_appearance font_width_appearance;

	//! Color for an unsupported font option, in one of the combo-boxes.
	text_color_arg unsupported_option_color;

	//! Horizontal scrollbar appearance around the font preview
	const_scrollbar_appearance preview_horizontal_scrollbar;

	//! Vertical scrollbar appearance around the font preview
	const_scrollbar_appearance preview_vertical_scrollbar;

	//! Generic popup configuration
	const_element_popup_appearance attached_popup_appearance;
};

//! Implementation object.

//! \see font_picker_appearance

class font_picker_appearanceObj: public font_picker_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	font_picker_appearanceObj();

	//! Destructor
	~font_picker_appearanceObj();

	//! Copy constructor
	font_picker_appearanceObj(const font_picker_appearanceObj &);

	//! Create a copy of this object.

	template<typename F> inline const_font_picker_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<
				 void(const font_picker_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_font_picker_appearance
	do_modify(const function<void(const font_picker_appearance &)> &) const;
};

LIBCXXW_NAMESPACE_END

#endif
