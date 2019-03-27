/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/input_field_appearance.H"
#include "x/w/combobox_appearance.H"
#include "x/w/scrollbar_appearance.H"

LIBCXXW_NAMESPACE_START

static button_config create_spinner_button_config()
{
	auto c=normal_button();
	c.normal_color="button_spinner_normal_color";
	c.selected_color="button_spinner_selected_color";
	c.active_color="button_spinner_active_color";

	return c;
}

input_field_appearance_properties::input_field_appearance_properties()
	: border{"textedit_border"},
	  focusoff_border{"texteditfocusoff_border"},
	  focuson_border{"texteditfocuson_border"},
	  foreground_color{"textedit_foreground_color"},
	  regular_font{theme_font{"textedit"}},
	  password_font{theme_font{"password"}},
	  background_color{"textedit_background_color"},
	  disabled_background_color{"textedit_disabled_background_color"},
	  hint_color{"textedit_hint_color"},
	  drag_horiz_start{"drag_horiz_start"},
	  drag_vert_start{"drag_vert_start"},
	  spinner_button_config{create_spinner_button_config()},
	  search_popup_appearance{combobox_appearance::base::theme()},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
{
}

input_field_appearance_properties::~input_field_appearance_properties()=default;

input_field_appearanceObj::input_field_appearanceObj()=default;

input_field_appearanceObj::~input_field_appearanceObj()=default;

input_field_appearanceObj::input_field_appearanceObj
(const input_field_appearanceObj &o)
	: input_field_appearance_properties{o}
{
}

const_input_field_appearance input_field_appearanceObj
::do_modify(const function<void (const input_field_appearance &)> &closure)
	const
{
	auto copy=input_field_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_input_field_appearance &input_field_appearance_base::theme()
{
	static const const_input_field_appearance config=
		const_input_field_appearance::create();

	return config;
}

static const_input_field_appearance create_editable_combobox_theme()
{
	auto custom=input_field_appearance::create();

	// The editable combo-box has its own border, no need for the
	// input field to add its own.
	custom->border={};

	return custom;
}

const const_input_field_appearance &
input_field_appearance_base::editable_combobox_theme()
{
	static const const_input_field_appearance config=
		create_editable_combobox_theme();

	return config;
}


LIBCXXW_NAMESPACE_END
