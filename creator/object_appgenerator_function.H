/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_object_appgenerator_function_h
#define creator_object_appgenerator_function_h

#include "creator/object_appgenerator_functionfwd.H"
#include "creator/object_appgenerator_functionobj.H"
#include "creator/appgenerator_function.H"
#include "creator/parameter_value.H"
#include "creator/parse_parameter.H"
#include <x/ref.H>
#include <x/xml/writelock.H>

//! Implementation object.

//! \see object_generator_value

struct object_generator_valueObj : public parameter_value,
				   virtual public x::obj {

public:
	object_generator_valueObj(parameter_value &&v);

	using parameter_value::operator=;
};

#endif
