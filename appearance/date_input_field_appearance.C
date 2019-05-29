/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/date_input_field_appearance.H"
#include "x/w/generic_window_appearance.H"
#include "x/w/image_button_appearance.H"
#include "x/w/input_field_appearance.H"
#include <x/w/focus_border_appearance.H>
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

static const_input_field_appearance create_date_input_field_appearance()
{
	auto custom=input_field_appearance::base::theme()
		->modify([]
			 (const auto &custom)
			 {
				 custom->border={};
				 custom->background_color=
					 "dateedit_background_color";
			 });
	return custom;
}

static const_input_field_appearance &default_date_input_field_appearance()
{
	static const_input_field_appearance
		config=create_date_input_field_appearance();
	return config;
}

date_input_field_appearance_properties::date_input_field_appearance_properties()
	: popup_button_image1{"scroll-right1"},
	  popup_button_image2{"scroll-right2"},
	  popup_button_focus_border{focus_border_appearance::base::dateeditbutton_theme()},
	  day_highlight_fg{theme_color{"dateedit_day_highlight_fg"}},
	  day_highlight_bg{theme_color{"dateedit_day_highlight_bg"}},
	  month_font{theme_font{"dateedit_popup_month"}},
	  month_color{theme_color{"dateedit_popup_month"}},
	  day_of_week_font{theme_font{"dateedit_day_of_week"}},
	  day_of_week_font_color{theme_color{"dateedit_day_of_week"}},
	  previous_year_appearance{image_button_appearance::base
			  ::date_popup_left_theme()},
	  previous_month_appearance{image_button_appearance::base
			  ::date_popup_left_theme()},
	  next_month_appearance{image_button_appearance::base
			  ::date_popup_right_theme()},
	  next_year_appearance{image_button_appearance::base
			  ::date_popup_right_theme()},
	  yscroll_height{"dateedit_popup_yscroll_height"},
	  mscroll_height{"dateedit_popup_mscroll_height"},
	  input_appearance{default_date_input_field_appearance()},
	  input_field_font{theme_font{"dateedit"}},
	  input_field_font_color{theme_color{"dateedit_foreground_color"}},
	  border{"dateedit_border"},
	  popup_border{"dateedit_popup_border"},
	  popup_background_color{"dateedit_popup_background_color"},
	  toplevel_appearance{generic_window_appearance::base
			      ::date_input_field_theme()}
{
}

date_input_field_appearance_properties::~date_input_field_appearance_properties()=default;

date_input_field_appearanceObj::date_input_field_appearanceObj()=default;

date_input_field_appearanceObj::~date_input_field_appearanceObj()=default;

date_input_field_appearanceObj::date_input_field_appearanceObj
(const date_input_field_appearanceObj &o)
	: date_input_field_appearance_properties{o}
{
}

const_date_input_field_appearance date_input_field_appearanceObj
::do_modify(const function<void (const date_input_field_appearance &)> &closure) const
{
	auto copy=date_input_field_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct date_input_field_appearance_base_themeObj : virtual public obj {

	const const_date_input_field_appearance config=const_date_input_field_appearance::create();

};

#if 0
{
#endif
}

const_date_input_field_appearance date_input_field_appearance_base::theme()
{
	return singleton<date_input_field_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
