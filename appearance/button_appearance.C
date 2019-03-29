/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/button_appearance.H"

LIBCXXW_NAMESPACE_START

button_appearance_properties::button_appearance_properties()
	: button_font{theme_font{"button"}},
	  normal_color{"button_normal_color"},
	  selected_color{"button_selected_color"},
	  active_color{"button_active_color"},

	  left_border{"normal_button_border"},
	  right_border{"normal_button_border"},
	  top_border{"normal_button_border"},
	  bottom_border{"normal_button_border"},

	  inputfocusoff_border{"inputfocusoff_border"},
	  inputfocuson_border{"inputfocuson_border"}
{
}

button_appearance_properties::~button_appearance_properties()=default;

button_appearanceObj::button_appearanceObj()=default;

button_appearanceObj::~button_appearanceObj()=default;

button_appearanceObj::button_appearanceObj
(const button_appearanceObj &o)
	: button_appearance_properties{o}
{
}

const_button_appearance button_appearanceObj
::do_modify(const function<void (const button_appearance &)> &closure) const
{
	auto copy=button_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_button_appearance &button_appearance_base::normal_theme()
{
	static const const_button_appearance config=
		const_button_appearance::create();

	return config;
}

const const_button_appearance &button_appearance_base::default_theme()
{
	static const const_button_appearance &config=
		normal_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->left_border=
				 appearance->right_border=
				 appearance->top_border=
				 appearance->bottom_border=
				 "default_button_border";
		 });

	return config;
}

LIBCXXW_NAMESPACE_END
