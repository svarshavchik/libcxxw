/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_window_appearance_h
#define x_w_generic_window_appearance_h

#include <x/w/generic_window_appearancefwd.H>
#include <x/w/generic_window_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref generic_window_appearance "generic_window appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::generic_window_appearance::base.

class generic_window_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_generic_window_appearance main_window_theme();

	//! Default theme.

	static const_generic_window_appearance date_input_field_theme();

	//! Default theme.

	static const_generic_window_appearance list_contents_theme();

	//! Default theme.

	static const_generic_window_appearance combobox_theme();

	//! Default theme.

	static const_generic_window_appearance tooltip_theme();
};

LIBCXXW_NAMESPACE_END

#endif
