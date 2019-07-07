/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/print_dialog_appearance.H"
#include "x/w/list_appearance.H"
#include "x/w/image_button_appearance.H"
#include "x/w/input_field_appearance.H"
#include "x/w/combobox_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

print_dialog_appearance_properties::print_dialog_appearance_properties()
	: printer_remote_font{theme_font{"printer_remote_font"}},
	  printer_local_font{theme_font{"printer_local_font"}},
	  printer_make_and_model_font{theme_font{"printer_make_and_model"}},
	  printer_location_font{theme_font{"printer_location"}},
	  printer_status_font{theme_font{"printer_status"}}
{
}

print_dialog_appearance_properties::~print_dialog_appearance_properties()=default;

print_dialog_appearanceObj::print_dialog_appearanceObj()=default;

print_dialog_appearanceObj::~print_dialog_appearanceObj()=default;

print_dialog_appearanceObj::print_dialog_appearanceObj
(const print_dialog_appearanceObj &o)
	: print_dialog_appearance_properties{o}
{
}

const_print_dialog_appearance print_dialog_appearanceObj
::do_modify(const function<void (const print_dialog_appearance &)> &closure) const
{
	auto copy=print_dialog_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct print_dialog_appearance_base_themeObj : virtual public obj {

	const const_print_dialog_appearance config=const_print_dialog_appearance::create();

};

#if 0
{
#endif
}

const_print_dialog_appearance print_dialog_appearance_base::theme()
{
	return singleton<print_dialog_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
