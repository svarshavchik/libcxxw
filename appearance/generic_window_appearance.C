/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/generic_window_appearance.H"

LIBCXXW_NAMESPACE_START

generic_window_appearance_properties::generic_window_appearance_properties()
	: label_font{theme_font{"label"}},
	  label_color{"label_foreground_color"},
	  modal_shade_color{"modal_shade"},
	  disabled_mask{"disabled_mask"},
	  wait_cursor{"cursor-wait"},
	  icons{
		{"mainwindow-icon", 16, 16},
		{"mainwindow-icon", 24, 24},
		{"mainwindow-icon", 32, 32},
		{"mainwindow-icon", 48, 48}
	  }
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

const const_generic_window_appearance
&generic_window_appearance_base::main_window_theme()
{
	static const const_generic_window_appearance config=
		const_generic_window_appearance::create();

	return config;
}


const const_generic_window_appearance
&generic_window_appearance_base::date_input_field_theme()
{
	static const_generic_window_appearance config=
		main_window_theme()->modify
		([]
		 (const auto &custom)
		 {
			 custom->label_font=theme_font{"dateedit_day"};
			 custom->label_color="dateedit_day";
		 });

	return config;
}

const const_generic_window_appearance
&generic_window_appearance_base::list_contents_theme()
{
	static const const_generic_window_appearance config=
		main_window_theme()->modify
		([]
		 (const auto &custom)
		 {
			 custom->label_font=theme_font{"list"};
		 });

	return config;
}

const const_generic_window_appearance
&generic_window_appearance_base::combobox_theme()
{
	static const const_generic_window_appearance config=
		main_window_theme()->modify
		([]
		 (const auto &custom)
		 {
			 custom->label_font=theme_font{"combobox"};
		 });

	return config;
}

const const_generic_window_appearance
&generic_window_appearance_base::tooltip_theme()
{
	static const const_generic_window_appearance config=
		main_window_theme()
		->modify([]
			 (const auto &custom)
			 {
				 custom->label_font=theme_font{"tooltip"};
			 });

	return config;
}

LIBCXXW_NAMESPACE_END