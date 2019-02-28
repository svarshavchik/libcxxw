/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_constructor_params_h
#define window_handler_constructor_params_h

#include "x/w/screenfwd.H"
#include "x/w/rgb.H"
#include "shared_handler_datafwd.H"

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN main_window_handler_constructor_params;

//! Parameters to main_windowObj::handlerObj's constructor.

//! These parameters get forwarded to its generic_windowObj::handlerObj, as is.

struct main_window_handler_constructor_params {
	//! New main window's parent screen.
	const screen &parent_screen;

	//! Comma-delimited _NET_WM_TYPE keywords, see set_window_type().
	const char *window_type;

	//! Comma-delimited _NET_WM_STATE keywords, see set_state()
	const char *window_state;

	//! New main window's background color.
	const color_arg &background_color;
};

LIBCXXW_NAMESPACE_END

#endif
