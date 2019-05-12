/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/button_config.H"
#include "x/w/button_appearance.H"
#include "x/w/input_field_appearance.H"
#include "x/w/combobox_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/focus_border_appearance.H"

LIBCXXW_NAMESPACE_START

static const_button_appearance
create_left_spinner_button_appearance(const border_arg &border)
{
	return normal_button().appearance->modify
		([&]
		 (const auto &appearance)
		 {
			 appearance->normal_color=
				 "button_spinner_normal_color";
			 appearance->selected_color=
				 "button_spinner_selected_color";
			 appearance->active_color=
				 "button_spinner_active_color";


			 appearance->left_border=
				 appearance->right_border=border_arg{};
			 appearance->top_border=appearance->bottom_border=
				 border;
		 });
}

static const_button_appearance
create_right_spinner_button_appearance(const border_arg &border)
{
	return normal_button().appearance->modify
		([&]
		 (const auto &appearance)
		 {
			 appearance->normal_color=
				 "button_spinner_normal_color";
			 appearance->selected_color=
				 "button_spinner_selected_color";
			 appearance->active_color=
				 "button_spinner_active_color";

			 appearance->left_border={};
			 appearance->right_border=
				 appearance->top_border=
				 appearance->bottom_border=border;
		 });
}


input_field_appearance_properties::input_field_appearance_properties()
	: invisible_pointer{"cursor-invisible"},
	  dragging_pointer{"cursor-dragging"},
	  dragging_nodrop_pointer{"cursor-dragging-wontdrop"},
	  border{"textedit_border"},
	  focus_border{focus_border_appearance::base::input_field_theme()},
	  foreground_color{"textedit_foreground_color"},
	  regular_font{theme_font{"textedit"}},
	  password_font{theme_font{"password"}},
	  background_color{"textedit_background_color"},
	  disabled_background_color{"textedit_disabled_background_color"},
	  hint_color{"textedit_hint_color"},
	  drag_horiz_start{"drag_horiz_start"},
	  drag_vert_start{"drag_vert_start"},
	  left_spinner_appearance{create_left_spinner_button_appearance
				  (border)},
	  right_spinner_appearance{create_right_spinner_button_appearance
				   (border)},
	  search_popup_appearance{combobox_appearance::base::theme()},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()},
	  spin_decrement{"spin-decrement"},
	  spin_increment{"spin-increment"}
{
}

void input_field_appearanceObj::update_spinner_buttons()
{
	left_spinner_appearance=create_left_spinner_button_appearance(border);
	right_spinner_appearance=create_right_spinner_button_appearance(border);
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
