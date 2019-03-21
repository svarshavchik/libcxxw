/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/popup_list_appearance.H"

LIBCXXW_NAMESPACE_START

// Default constructor is for combo-box properties.

popup_list_appearance_properties::popup_list_appearance_properties()
	: popup_border{"combobox_popup_border"},
	  topleft_color{"combobox_above_background_color"},
	  bottomright_color{"combobox_below_background_color"},
	  modal_shade_color{"modal_shade"}
{
}

popup_list_appearance_properties::~popup_list_appearance_properties()=default;

popup_list_appearanceObj::popup_list_appearanceObj()
{
	background_color="combobox_background_color";
	selected_color="combobox_selected_color";
	highlighted_color="combobox_highlighted_color";
	current_color="combobox_current_color";
	list_font=theme_font{"combobox"};
}

popup_list_appearanceObj::~popup_list_appearanceObj()=default;

popup_list_appearanceObj
::popup_list_appearanceObj(const popup_list_appearanceObj &c)
	: list_appearanceObj{static_cast<const list_appearance_properties &>
		(c)},
	  popup_list_appearance_properties{c}
{
}

popup_list_appearance popup_list_appearanceObj::clone() const
{
	return popup_list_appearance::create(*this);
}

LIBCXXW_NAMESPACE_END
