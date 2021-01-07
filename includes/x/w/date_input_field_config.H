/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_config_h
#define x_w_date_input_field_config_h

#include <x/w/types.H>
#include <x/w/date_input_field_configfwd.H>
#include <x/w/date_input_field_appearancefwd.H>
#include <x/w/text_param.H>

LIBCXXW_NAMESPACE_START

//! Custom visual appearance of a \ref date_input_field "date input field".

struct date_input_field_config {

	//! Constructor
	date_input_field_config();

	//! Destructor

	~date_input_field_config();

	//! Copy constructor
	date_input_field_config(const date_input_field_config &);

	//! Assignment operator
	date_input_field_config &operator=(const date_input_field_config &);

	//! Default appearance of the input field.

	//! Cached appearance object, use the current display theme.
	const_date_input_field_appearance appearance;

	//! Error message to show for invalid input.

	text_param invalid_input;
};

LIBCXXW_NAMESPACE_END

#endif
