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
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

color_picker_appearance_properties::color_picker_appearance_properties()
	: width{"color_picker_current_width"},
	  height{"color_picker_current_height"},
	  attached_popup_appearance{const_element_popup_appearance::base
				    ::theme()},
	  picker_width{"color_picker_square_width"},
	  picker_height{"color_picker_square_height"}
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

namespace {
#if 0
}
#endif

struct color_picker_appearance_base_themeObj : virtual public obj {

	const const_color_picker_appearance config=const_color_picker_appearance::create();

};

#if 0
{
#endif
}

const_color_picker_appearance color_picker_appearance_base::theme()
{
	return singleton<color_picker_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
