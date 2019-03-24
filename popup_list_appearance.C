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
	  bottomright_color{"combobox_below_background_color"},
	  topleft_color{"combobox_above_background_color"},
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

static popup_list_appearance create_menu_theme()
{
	auto appearance=popup_list_appearance::create();

	appearance->popup_border="menu_popup_border";
	appearance->topleft_color="menu_above_background_color";
	appearance->bottomright_color="menu_below_background_color";

	appearance->h_padding="menu_list_h_padding";
	appearance->v_padding="menu_list_v_padding";
	appearance->list_foreground_color="menu_popup_foreground_color";
	appearance->selected_color="menu_popup_selected_color";
	appearance->current_color="menu_popup_highlighted_color";
	appearance->highlighted_color="menu_popup_clicked_color";
	appearance->list_font=theme_font{"menu_font"};

	return appearance;
}

const const_popup_list_appearance &popup_list_appearance_base::menu_theme()
{
	static const const_popup_list_appearance config=create_menu_theme();

	return config;
}

static popup_list_appearance create_submenu_theme()
{
	auto appearance=create_menu_theme();

	appearance->bottomright_color="menu_right_background_color";
	appearance->topleft_color="menu_left_background_color";

	return appearance;
}

const const_popup_list_appearance &popup_list_appearance_base::submenu_theme()
{
	static const const_popup_list_appearance config=create_submenu_theme();

	return config;
}

const const_popup_list_appearance &popup_list_appearance_base::contextmenu_theme()
{
	return menu_theme();
}

LIBCXXW_NAMESPACE_END
