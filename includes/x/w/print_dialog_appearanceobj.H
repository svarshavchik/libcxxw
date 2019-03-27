/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialog_appearanceobj_h
#define x_w_print_dialog_appearanceobj_h

#include <x/w/print_dialog_appearancefwd.H>
#include <x/w/list_appearancefwd.H>
#include <x/w/image_button_appearancefwd.H>
#include <x/w/input_field_appearancefwd.H>
#include <x/w/combobox_appearancefwd.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref print_dialog_appearance "INSERT_LIBX_NAMESPACE::w::print_dialog_appearance".

struct print_dialog_appearance_properties {

	//! Constructor
	print_dialog_appearance_properties();

	//! Destructor
	~print_dialog_appearance_properties();

	//! Font of names of remote printers.
	font_arg printer_remote_font;

	//! Font of names of local printers.
	font_arg printer_local_font;

	//! Font of printer make and model
	font_arg printer_make_and_model_font;

	//! Font of printer locations
	font_arg printer_location_font;

	//! Font of printer status
	font_arg printer_status_font;

	//! Appearance of the list of printers to select.
	const_list_appearance printer_field_appearance;

	//! Appearance of the number of copies field.
	const_input_field_appearance number_of_copies_appearance;

	//! Appearance of the "All pages" radio button
	const_image_button_appearance print_all_pages_appearance;

	//! Appearance of the "Pages" radio button
	const_image_button_appearance print_page_range_appearance;

	//! Appearance of the "Pages" field
	const_input_field_appearance page_range_appearance;

	//! Appearance of the "Orientation" list
	const_combobox_appearance orientation_appearance;

	//! Appearance of the "Duplex" list
	const_combobox_appearance duplex_appearance;

	//! Appearance of the "Pages per side" list
	const_combobox_appearance pages_per_side_appearance;

	//! Appearance of the "Page size" list
	const_combobox_appearance page_size_appearance;

	//! Appearance of the "Finishing process" list
	const_combobox_appearance finishings_appearance;

	//! Appearance of the "Color Mode" list
	const_combobox_appearance print_color_mode_appearance;

	//! Appearance of the "Print Quality" list
	const_combobox_appearance print_quality_appearance;

	//! Appearance of the "Resolution" list
	const_combobox_appearance printer_resolution_appearance;
};

//! Implementation object.

//! \see print_dialog_appearance

class print_dialog_appearanceObj: public print_dialog_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	print_dialog_appearanceObj();

	//! Destructor
	~print_dialog_appearanceObj();

	//! Copy constructor
	print_dialog_appearanceObj(const print_dialog_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_print_dialog_appearance modify(F &&f)
		const
	{
		return do_modify(make_function<void(const print_dialog_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_print_dialog_appearance
	do_modify(const function<void(const print_dialog_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
