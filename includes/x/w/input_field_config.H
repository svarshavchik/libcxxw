/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_config_h
#define x_w_input_field_config_h

#include <cstdint>

#include <x/w/types.H>
#include <x/w/alignment.H>
#include <x/w/input_field_configfwd.H>

LIBCXXW_NAMESPACE_START

//! The configuration of an input field.

struct input_field_config {

	//! Number of columns
	size_t columns=40;

	//! Number of rows
	size_t rows=1;

	//! Inner alignment
	halign alignment=halign::left;

	//! Single line input field.
	constexpr bool oneline() const { return rows <= 1; }
};

LIBCXXW_NAMESPACE_END

#endif
