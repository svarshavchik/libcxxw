/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_constructor_params_h
#define window_handler_constructor_params_h

#include "x/w/screenfwd.H"
#include "x/w/rgb.H"
#include "x/w/font_arg.H"
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

	//! Default font
	const font_arg &default_label_font;

	//! Default color
	const color_arg &default_label_color;

	//! Shaded color
	const color_arg &modal_shade_color;
};

struct LIBCXX_HIDDEN generic_window_handler_constructor_params;

//! Parameters to generic_windowObj::handlerObj's constructor.

//! The parameters include the parameters to its subclass, main_window_params,
//! or a reasonable facsimile for other subclasses.

struct generic_window_handler_constructor_params {

	//! Parent screen, background color, and window manager stuff.

	const main_window_handler_constructor_params &main_window_params;

	//! Shared data between popups and their main application window.
	shared_handler_data handler_data;

	//! Relative nesting level of this top-level window.

	//! 0 for main application windows. For popups attached to display
	//! elements this gets set based on the nesting level of the
	//! display element the popup is attached to.
	size_t nesting_level;

	//! Create this window with the override-redirect flag
	bool override_redirect;
};


LIBCXXW_NAMESPACE_END

#endif
