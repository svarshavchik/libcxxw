/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef window_handler_constructor_params_h
#define window_handler_constructor_params_h

#include "x/w/screenfwd.H"
#include "x/w/rgb.H"
#include "x/w/font_arg.H"
#include "x/w/generic_window_appearancefwd.H"
#include "x/w/main_window_appearancefwd.H"
#include "x/w/screen_positionsfwd.H"
#include "x/w/impl/screen_positions_confighandlefwd.H"
#include "shared_handler_datafwd.H"
#include <functional>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN main_window_handler_constructor_params;

struct LIBCXX_HIDDEN generic_window_handler_constructor_params;

//! Parameters to generic_windowObj::handlerObj's constructor.

//! The parameters include the parameters to its subclass, main_window_params,
//! or a reasonable facsimile for other subclasses.

struct generic_window_handler_constructor_params {

	//! New main window's parent screen.
	const screen &parent_screen;

	//! Comma-delimited _NET_WM_TYPE keywords, see set_window_type().
	const char *window_type;

	//! Comma-delimited _NET_WM_STATE keywords, see set_state()
	const char *window_state;

	//! New main window's background color.
	const color_arg &background_color;

	//! Appearance
	const const_generic_window_appearance &appearance;

	//! Shared data between popups and their main application window.
	shared_handler_data handler_data;

	//! Saved screen positions
	const screen_positions &positions;

	//! Handle for this main window's saved position
	const screen_positions_handle &config_handle;

	//! What we will put into WM_CLASS
	const std::string &wm_class_instance;

	//! What we will put into WM_CLASS
	const std::string &wm_class_resource;

	//! Relative nesting level of this top-level window.

	//! 0 for main application windows. For popups attached to display
	//! elements this gets set based on the nesting level of the
	//! display element the popup is attached to.
	size_t nesting_level;

	//! Create this window with the override-redirect flag
	bool override_redirect;
};

//! Parameters to main_windowObj::handlerObj's constructor.

//! These parameters get forwarded to its generic_windowObj::handlerObj, as is.
//!
//! Explicit reference_wrappers ensure that they won't bind to temporary
//! values.

struct main_window_handler_constructor_params
	: generic_window_handler_constructor_params {

	main_window_handler_constructor_params
	(const std::reference_wrapper<const screen> &parent_screen,
	 const char *window_type,
	 const char *window_state,
	 const std::reference_wrapper<const color_arg> &background_color,
	 const std::reference_wrapper<const const_main_window_appearance>
	 &appearance,
	 const std::reference_wrapper<const screen_positions> &positions,
	 const std::reference_wrapper<const screen_positions_handle>
	 &config_handle,
	 const std::reference_wrapper<const std::string> &wm_class_instance,
	 bool override_redirect);

	//! Appearance
	const const_main_window_appearance &appearance;
};


LIBCXXW_NAMESPACE_END

#endif
