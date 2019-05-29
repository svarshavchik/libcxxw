/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/button_appearance.H"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>

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
	  focus_border{focus_border_appearance::base::theme()}
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

namespace {
#if 0
}
#endif

struct button_appearance_base_normal_themeObj : virtual public obj {

	const const_button_appearance config=const_button_appearance::create();

};

#if 0
{
#endif
}

const_button_appearance button_appearance_base::normal_theme()
{
	return singleton<button_appearance_base_normal_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct button_appearance_base_default_themeObj : virtual public obj {

	const const_button_appearance config=
		button_appearance_base::normal_theme()->modify
		([]
		 (const auto &appearance)
		 {
			 appearance->left_border=
				 appearance->right_border=
				 appearance->top_border=
				 appearance->bottom_border=
				 "default_button_border";
		 });

};

#if 0
{
#endif
}

const_button_appearance button_appearance_base::default_theme()
{
	return singleton<button_appearance_base_default_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
