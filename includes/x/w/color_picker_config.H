/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_config_h
#define x_w_color_picker_config_h

#include <x/w/color_picker_configfwd.H>
#include <x/w/color_pickerfwd.H>
#include <x/w/element_popup_config.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Initial color picker configuration

//! An optional parameter to a \ref factory "factory"'s
//! create_color_picker() setting the initial value and apperance of a
//! \ref color_picker "INSERT_LIBX_NAMESPACE::w::color_picker".

struct color_picker_config {

	//! Initial callback
	functionptr<color_picker_callback_t> initial_callback;

	//! Initial color
	rgb initial_color=rgb{rgb::maximum, rgb::maximum, rgb::maximum};

	//! Popup configuration
	element_popup_config popup_config;
};

LIBCXXW_NAMESPACE_END

#endif
