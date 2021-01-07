/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/popup_list_appearance.H"
#include "x/w/generic_window_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

// Default constructor is for combo-box properties.

popup_list_appearance_properties::popup_list_appearance_properties()
	: popup_border{"combobox_popup_border"},
	  bottomright_color{"combobox_below_background_color"},
	  topleft_color{"combobox_above_background_color"}
{
}

popup_list_appearance_properties::~popup_list_appearance_properties()=default;

popup_list_appearanceObj::popup_list_appearanceObj()
{
	background_color="combobox_background_color";
	selected_color="combobox_selected_color";
	highlighted_color="combobox_highlighted_color";
	current_color="combobox_current_color";

	contents_appearance=generic_window_appearance::base::combobox_theme();
}

popup_list_appearanceObj::~popup_list_appearanceObj()=default;

popup_list_appearanceObj
::popup_list_appearanceObj(const popup_list_appearanceObj &c)
	: list_appearanceObj{c},
	  popup_list_appearance_properties{c}
{
}

const_popup_list_appearance popup_list_appearanceObj
::do_modify(const function<void (const popup_list_appearance &)> &closure) const
{
	auto copy=popup_list_appearance::create(*this);
	closure(copy);
        return copy;
}

static popup_list_appearance create_menu_theme()
{
	auto appearance=popup_list_appearance::create();

	appearance->popup_border="menu_popup_border";
	appearance->topleft_color="menu_above_background_color";
	appearance->bottomright_color="menu_below_background_color";

	appearance->h_padding="menu_list_h_padding";
	appearance->v_padding="menu_list_v_padding";
	appearance->contents_appearance=appearance->contents_appearance
		->modify([]
			 (const auto &custom)
			 {
				 custom->label_color=
					 "menu_popup_foreground_color";
				 custom->label_font=theme_font{"menu_font"};
			 });
	appearance->selected_color="menu_popup_selected_color";
	appearance->current_color="menu_popup_highlighted_color";
	appearance->highlighted_color="menu_popup_clicked_color";

	return appearance;
}

namespace {
#if 0
}
#endif

struct popup_list_appearance_base_menu_themeObj : virtual public obj {

	const const_popup_list_appearance config=create_menu_theme();
};

#if 0
{
#endif
}

const_popup_list_appearance popup_list_appearance_base::menu_theme()
{
	return singleton<popup_list_appearance_base_menu_themeObj>::get()->config;
}

static popup_list_appearance create_submenu_theme()
{
	auto appearance=create_menu_theme();

	appearance->bottomright_color="menu_right_background_color";
	appearance->topleft_color="menu_left_background_color";

	return appearance;
}

namespace {
#if 0
}
#endif

struct popup_list_appearance_base_submenu_themeObj : virtual public obj {

	const const_popup_list_appearance config=create_submenu_theme();
};

#if 0
{
#endif
}

const_popup_list_appearance popup_list_appearance_base::submenu_theme()
{
	return singleton<popup_list_appearance_base_submenu_themeObj>::get()->config;
}

const_popup_list_appearance popup_list_appearance_base::contextmenu_theme()
{
	return menu_theme();
}

LIBCXXW_NAMESPACE_END
