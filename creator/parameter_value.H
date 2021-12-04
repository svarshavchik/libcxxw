/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_parameter_value_h
#define creator_parameter_value_h

#include "creator/appgenerator_functionsfwd.H"
#include <x/vector.H>
#include <string>
#include <vector>
#include <variant>

struct parameter_value;

//! Additional information associated with a parameter to a function or an object member

//! Additional metada associated with a \ref parameter_value "parameter value".
//!
//! The layout manager generator stores the currently-selected layout manager
//! and its compiled functions.
//!
//! A generator for a specific set of function stores just its functions here.
typedef std::variant<std::monostate,
		     existing_appgenerator_functions,
		     std::vector<const_appgenerator_function>
		     > parameter_value_extra_t;

//! The value of a particular generator
struct parameter_value {

	//! The actual value, that gets interpreted in some way.
	std::u32string string_value;

	//! Additional values, that depend on the parameter type.
	//!
	//! The value for the layoutmanager_type includes the
	//! existing_appgenerator_functions for the layoutmanager_type's
	//! container's <config>.
	//!
	//! An <object> parameter will have a vector of current values, parsed
	//! as generator functions.

	parameter_value_extra_t extra;

	//! This value has been clone()d

	//! Clone x::vector<parameter_value> extras.

	void cloned();
};

#endif
