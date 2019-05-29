/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/generic_window_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

generic_window_appearance_properties::generic_window_appearance_properties()
	: label_font{theme_font{"label"}},
	  label_color{"label_foreground_color"},
	  modal_shade_color{"modal_shade"},
	  disabled_mask{"disabled_mask"},
	  wait_cursor{"cursor-wait"}
{
}

generic_window_appearance_properties::~generic_window_appearance_properties()=default;

generic_window_appearanceObj::generic_window_appearanceObj()=default;

generic_window_appearanceObj::~generic_window_appearanceObj()=default;

generic_window_appearanceObj::generic_window_appearanceObj
(const generic_window_appearanceObj &o)
	: generic_window_appearance_properties{o}
{
}

const_generic_window_appearance generic_window_appearanceObj
::do_modify(const function<void (const generic_window_appearance &)> &closure)
	const
{
	auto copy=generic_window_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct generic_window_appearance_base_main_window_themeObj : virtual public obj {

	const const_generic_window_appearance config=const_generic_window_appearance::create();

};

#if 0
{
#endif
}

const_generic_window_appearance
generic_window_appearance_base::main_window_theme()
{
	return singleton<generic_window_appearance_base_main_window_themeObj>::get()->config;
}


namespace {
#if 0
}
#endif

struct generic_window_appearance_base_date_input_field_themeObj : virtual public obj {

	const const_generic_window_appearance config=
		generic_window_appearance_base::main_window_theme()->modify
		([]
		 (const auto &custom)
		 {
			 custom->label_font=theme_font{"dateedit_day"};
			 custom->label_color="dateedit_day";
		 });

};

#if 0
{
#endif
}

const_generic_window_appearance
generic_window_appearance_base::date_input_field_theme()
{
	return singleton<generic_window_appearance_base_date_input_field_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct generic_window_appearance_base_list_contents_themeObj : virtual public obj {

	const const_generic_window_appearance config=
		generic_window_appearance_base::main_window_theme()->modify
		([]
		 (const auto &custom)
		 {
			 custom->label_font=theme_font{"list"};
		 });

};

#if 0
{
#endif
}

const_generic_window_appearance
generic_window_appearance_base::list_contents_theme()
{
	return singleton<generic_window_appearance_base_list_contents_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct generic_window_appearance_base_combobox_themeObj : virtual public obj {

	const const_generic_window_appearance config=
		generic_window_appearance_base::main_window_theme()->modify
		([]
		 (const auto &custom)
		 {
			 custom->label_font=theme_font{"combobox"};
		 });

};

#if 0
{
#endif
}

const_generic_window_appearance
generic_window_appearance_base::combobox_theme()
{
	return singleton<generic_window_appearance_base_combobox_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct generic_window_appearance_base_tooltip_themeObj : virtual public obj {

	const const_generic_window_appearance config=
		generic_window_appearance_base::main_window_theme()
		->modify([]
			 (const auto &custom)
			 {
				 custom->label_font=theme_font{"tooltip"};
			 });

};

#if 0
{
#endif
}

const_generic_window_appearance
generic_window_appearance_base::tooltip_theme()
{
	return singleton<generic_window_appearance_base_tooltip_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
