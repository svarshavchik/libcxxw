/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_button_appearanceobj_h
#define x_w_item_button_appearanceobj_h

#include <x/w/item_button_appearancefwd.H>
#include <x/w/image_button_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref item_button_appearance "INSERT_LIBX_NAMESPACE::w::item_button_appearance".

struct item_button_appearance_properties {

	//! Constructor
	item_button_appearance_properties();

	//! Destructor
	~item_button_appearance_properties();

	//! Border around buttons

	border_arg button_border;

	//! Padding between the border and the button's label

	dim_arg itembutton_h_padding;

	//! Padding between the border and the button's label

	dim_arg itembutton_v_padding;

	//! Button background color

	color_arg itembutton_background_color;

	//! Close image button appearance.
	const_image_button_appearance item_image_button_appearance;
};

//! Implementation object.

//! \see item_button_appearance

class item_button_appearanceObj: public item_button_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	item_button_appearanceObj();

	//! Destructor
	~item_button_appearanceObj();

	//! Copy constructor
	item_button_appearanceObj(const item_button_appearanceObj &);

	//! Create a copy of this object.
	item_button_appearance clone() const;
};

LIBCXXW_NAMESPACE_END

#endif
