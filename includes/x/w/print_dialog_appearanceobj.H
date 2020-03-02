/*
** Copyright 2019-2020 Double Precision, Inc.
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
#include <x/w/appearanceobj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref print_dialog_appearance "INSERT_LIBX_NAMESPACE::w::print_dialog_appearance".

struct print_dialog_appearance_properties {

	//! Constructor
	print_dialog_appearance_properties();

	//! Destructor
	~print_dialog_appearance_properties();

#include <x/w/print_dialog_appearance.inc.H>

};

//! Implementation object.

//! \see print_dialog_appearance

class print_dialog_appearanceObj: public print_dialog_appearance_properties,
				  public appearanceObj {

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
