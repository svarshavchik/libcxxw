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

LIBCXXW_NAMESPACE_START

print_dialog_appearance_properties::print_dialog_appearance_properties()
	: printer_remote_font{theme_font{"printer_remote_font"}},
	  printer_local_font{theme_font{"printer_local_font"}},
	  printer_make_and_model_font{theme_font{"printer_make_and_model"}},
	  printer_location_font{theme_font{"printer_location"}},
	  printer_status_font{theme_font{"printer_status"}},
	  printer_field_appearance{list_appearance::base::theme()},
	  number_of_copies_appearance{input_field_appearance::base::theme()},
	  print_all_pages_appearance{
		  image_button_appearance::base::radio_theme()},
	  print_page_range_appearance{
		  image_button_appearance::base::radio_theme()},
	  page_range_appearance{input_field_appearance::base::theme()},
	  orientation_appearance{combobox_appearance::base::theme()},
	  duplex_appearance{combobox_appearance::base::theme()},
	  pages_per_side_appearance{combobox_appearance::base::theme()},
	  page_size_appearance{combobox_appearance::base::theme()},
	  finishings_appearance{combobox_appearance::base::theme()},
	  print_color_mode_appearance{combobox_appearance::base::theme()},
	  print_quality_appearance{combobox_appearance::base::theme()},
	  printer_resolution_appearance{combobox_appearance::base::theme()}
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

const const_print_dialog_appearance &print_dialog_appearance_base::theme()
{
	static const const_print_dialog_appearance config=
		const_print_dialog_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END