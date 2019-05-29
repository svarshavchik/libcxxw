/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_appearance_h
#define x_w_input_field_appearance_h

#include <x/w/input_field_appearancefwd.H>
#include <x/w/input_field_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref input_field_appearance "input_field appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::input_field_appearance::base.

class input_field_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_input_field_appearance theme();

	//! Default theme for the input field in an editable combo-box.

	static const_input_field_appearance editable_combobox_theme();

	//! Default theme for date input fields.

	static const_input_field_appearance date_input_field_theme();
};

LIBCXXW_NAMESPACE_END

#endif
