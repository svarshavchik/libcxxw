/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_setting_handlerfwd_h
#define creator_setting_handlerfwd_h

#include <x/functionalrefptr.H>
#include <x/w/focusable.H>
#include <x/xml/writelockfwd.H>
#include <x/xml/readlockfwd.H>
#include "creator/parameter_value.H"
#include <optional>
#include <string>

struct setting_handler;

typedef x::functionref< std::optional<parameter_value> (bool)
			> setting_create_ui_validator;

// What a setting's create_ui() returns, as described below.

struct setting_create_ui_ret_t {

	/*! Validate the setting's value.

	  If the setting value's is valid return it.

	  The parameter is true if an invalid value should be reported. If
	  false the setting value is still checked and returned, but no
	  error messages are shown.

	 */
	const setting_create_ui_validator validator;

	/*! Focusable widget

	  The setting's focusable widget. If other settings want to report
	  an error but return the input focus here, this is used for that.
	 */

	const x::w::focusableptr setting_focusable;
};

struct setting_save_info {
	const x::xml::writelock &lock;
	const std::string &parameter_name;
	const parameter_value &value;
	const std::string &handler_name;
	const bool is_member;
};

struct setting_load_info {
	x::xml::readlock lock;
	const std::string &parameter_name;
	parameter_value &value;
};

#endif
