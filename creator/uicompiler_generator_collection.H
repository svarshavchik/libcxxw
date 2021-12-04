/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef uicompiler_generator_collection_h
#define uicompiler_generator_collection_h

#include <x/logger.H>
#include <x/xml/readlockfwd.H>
#include "creator/parse_parameterfwd.H"
#include "creator/object_appgenerator_functionfwd.H"

#include <tuple>
#include <variant>
#include <vector>
#include <string>
#include <unordered_map>

struct parameter_value;

//! Common code inherited by parse_function and parse_parameter

//! A function contains parameters. A parameter can be an object parameter
//! with members, which are basically nested parameters. This is common
//! shared logic

struct uicompiler_generator_collection {

	LOG_CLASS_SCOPE;

	//! This <condition> has an <exists> value.

	//! The bool flag indicates whether the given string exists as a
	//! discrete parameter. It's possible that <exists> names one of the
	//! parameters, or a different element that exists by itself.
	typedef std::tuple<std::string, bool> condition_exists;

	//! This parameter does not have a <condition>
	typedef std::monostate no_condition;

	//! This <condition> has a <name> and a <value>
	typedef std::tuple<std::string, std::string> condition_name_and_value;

	//! Whether or not this function has a <condition>
	std::variant<no_condition,
		     condition_name_and_value,
		     std::tuple<std::string, bool>> condition;

	//! All of this function's <parameter>s.

	//! Or all of these parameter's members.
	std::vector<parse_parameter> parsed_parameters;

	//! The function objects for the parsed_parameters

	//! Map keyed by parse_parameter->parameter_name

	std::unordered_map<std::string, const_object_appgenerator_function
			   > parsed_parameter_functions;

	//! Whether this is a parameter or a member.

	//! parse_parameterObj overrides and returns true.
	//! parse_function overrides and returns false.

	virtual bool is_parameter() const=0;

	//! Return the paremeter's name

	//! For logging purposes.

	virtual std::string get_name() const=0;

	//! Try to parse this function or object parameter.

	//! For an element in the theme file that specifies this function's
	//! name try to parse it. There can be several parsed function's
	//! with the given name, if they specify <condition>s.
	//!
	//! Returns true if the function was parsed and added to the "funcs"
	//! parameter. Returns false if the function's conditions are not met.
	//!
	//! Object parameters don't have conditions and always have
	//! condition=no_condition, so that part gets skipped.
	//!
	//! An empty vector of parameter values get passed in. If parsing
	//! succeeds these are the values for the parsed parameters.
	bool parse_function_or_object_generator(
		const x::xml::readlock &root,
		std::vector<parameter_value> &parameter_values
	) const;

	//! Prepare the function's object parameters' members.

	//! After creating a new std::vector of parameter_values, with the
	//! same size as the parsed_parameters vector we need to go ahead
	//! and create additional vectors of parameter_values for the
	//! parsed_parameters which are is_object_parameter-s, for their
	//! members.
	void prepare_object_member_values(std::vector<parameter_value> &values)
		const;
};

#endif
