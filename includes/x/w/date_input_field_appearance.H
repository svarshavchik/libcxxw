/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_appearance_h
#define x_w_date_input_field_appearance_h

#include <x/w/date_input_field_appearancefwd.H>
#include <x/w/date_input_field_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref date_input_field_appearance "date_input_field appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::date_input_field_appearance::base.

class date_input_field_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const const_date_input_field_appearance &theme();

};

LIBCXXW_NAMESPACE_END

#endif
