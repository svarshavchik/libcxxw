/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_setting_handlerfwd_h
#define creator_setting_handlerfwd_h

#include <x/functionalrefptr.H>
#include <x/xml/writelockfwd.H>
#include <x/xml/readlockfwd.H>
#include "creator/parameter_value.H"
#include <optional>
#include <string>

struct setting_handler;

// What a setting's create_ui() returns, as described below.

typedef x::functionref< std::optional<parameter_value> (bool)
			> setting_create_ui_ret_t;

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
