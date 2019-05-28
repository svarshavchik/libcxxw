/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/color_picker_appearance.H"
#include "x/w/element_popup_appearance.H"
#include "x/w/input_field_appearance.H"
#include "x/w/button_config.H"
#include "x/w/button_appearance.H"

LIBCXXW_NAMESPACE_START

color_picker_appearance_properties::color_picker_appearance_properties()
	: width{"color_picker_current_width"},
	  height{"color_picker_current_height"},
	  attached_popup_appearance{const_element_popup_appearance::base
				    ::theme()},
	  strip_width{"color_picker_strip_width"},
	  strip_height{"color_picker_strip_height"},
	  picker_width{"color_picker_square_width"},
	  picker_height{"color_picker_square_height"},
	  basic_color_width{"color_picker_basic_color_width"},
	  basic_color_height{"color_picker_basic_color_height"},
	  h_button_appearance{normal_button().appearance},
	  v_button_appearance{normal_button().appearance},
	  basic_colors_button_appearance{normal_button().appearance},
	  r_appearance{input_field_appearance::base::theme()},
	  g_appearance{input_field_appearance::base::theme()},
	  b_appearance{input_field_appearance::base::theme()},
	  h_appearance{input_field_appearance::base::theme()},
	  s_appearance{input_field_appearance::base::theme()},
	  v_appearance{input_field_appearance::base::theme()}
{
}

color_picker_appearance_properties::~color_picker_appearance_properties()=default;

color_picker_appearanceObj::color_picker_appearanceObj()=default;

color_picker_appearanceObj::~color_picker_appearanceObj()=default;

color_picker_appearanceObj::color_picker_appearanceObj
(const color_picker_appearanceObj &o)
	: color_picker_appearance_properties{o}
{
}

const_color_picker_appearance color_picker_appearanceObj
::do_modify(const function<void (const color_picker_appearance &)> &closure) const
{
	auto copy=color_picker_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_color_picker_appearance &color_picker_appearance_base::theme()
{
	static const const_color_picker_appearance config=
		const_color_picker_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
