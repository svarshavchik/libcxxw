/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef create_ui_info_h
#define create_ui_info_h

#include "creator/app.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "creator/parameter_value.H"

#include <string>

//! Parameters to a setting_handler's create_ui() call.

struct create_ui_info {

	// The application singleton
	appObj * app;

	// The parent window or dialog
	const x::w::main_window &mw;

	// The factory for the generator

	// The create_ui() handler must use this factory to create exactly
	// one widget that represents the value of this generator
	const x::w::gridfactory &value_factory;

	// The factory came from the name_value_lm. The grid contains two
	// columns: generator name, and generator value, which create_ui()
	// creates using the value_factory
	//
	// Some generators might have additional stuff to deal with, and will
	// create an additional row in the grid for them.
	const x::w::gridlayoutmanager &name_value_lm;

	// The value of this generator
	const parameter_value &value;

	// Parameter is required

	const bool required;

	// Parameter's name

	const std::string &parameter_name;

	// A name associated with a handler

	// The "lookup_factory" handler, for example, uses this to store the
	// name of the factory. Elsewhere this is used in error messages.
	const std::string &handler_name;
};

#endif
