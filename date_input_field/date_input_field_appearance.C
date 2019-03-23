/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/date_input_field_appearance.H"
#include "x/w/input_field_appearance.H"

LIBCXXW_NAMESPACE_START

static const_input_field_appearance create_date_input_field_appearance()
{
	auto custom=input_field_appearance::base::theme()->clone();

	custom->border={};
	custom->background_color="dateedit_background_color";
	return custom;
}

static const_input_field_appearance &default_date_input_field_appearance()
{
	static const_input_field_appearance
		config=create_date_input_field_appearance();
	return config;
}

date_input_field_appearance_properties::date_input_field_appearance_properties()
	: day_highlight_fg{theme_color{"dateedit_day_highlight_fg"}},
	  day_highlight_bg{theme_color{"dateedit_day_highlight_bg"}},
	  month_font{theme_font{"dateedit_popup_month"}},
	  month_color{theme_color{"dateedit_popup_month"}},
	  day_of_week_font{theme_font{"dateedit_day_of_week"}},
	  day_of_week_font_color{theme_color{"dateedit_day_of_week"}},
	  yscroll_height{"dateedit_popup_yscroll_height"},
	  mscroll_height{"dateedit_popup_mscroll_height"},
	  input_appearance{default_date_input_field_appearance()},
	  input_field_font{theme_font{"dateedit"}},
	  input_field_font_color{theme_color{"dateedit_foreground_color"}},
	  border{"dateedit_border"},
	  focusoff_border{"dateeditbuttonfocusoff_border"},
	  focuson_border{"dateeditbuttonfocuson_border"},
	  popup_border{"dateedit_popup_border"},
	  popup_background_color{"dateedit_popup_background_color"},
	  popup_font{theme_font{"dateedit_day"}},
	  popup_font_color{"dateedit_day"},
	  popup_modal_shade_color{"modal_shade"}
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

date_input_field_appearance date_input_field_appearanceObj::clone() const
{
	return date_input_field_appearance::create(*this);
}

const const_date_input_field_appearance &date_input_field_appearance_base::theme()
{
	static const const_date_input_field_appearance config=
		const_date_input_field_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
