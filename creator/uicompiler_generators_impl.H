/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generators_impl_h
#define creator_uicompiler_generators_impl_h

#include "creator/uicompiler_generators.H"
#include "creator/uicompiler_generator_collection.H"
#include "creator/appgenerator_functionsfwd.H"
#include "creator/appgenerator_savefwd.H"
#include "creator/parse_parameter.H"
#include "creator/parameter_value.H"
#include "x/w/text_param.H"
#include <x/obj.H>
#include <x/visitor.H>
#include <x/vector.H>
#include <x/xml/readlock.H>
#include <unordered_map>
#include <string>
#include <vector>
#include <variant>
#include <tuple>

struct parse_function;

//! Parsed \ref "uicompiler_generators" information

//! Internal data, parsed contents of a <function> specified in uicompiler.xml

struct parse_function : uicompiler_generator_collection {

	//! The function's name.

	std::string name;

	//! Parse this <function> in uicompiler.xml
	parse_function(const x::xml::readlock &root);

	//! Implement is_parameter().

	bool is_parameter() const override;

	//! Implement get_name().
	std::string get_name() const override;

	//! Give a description of tihs function.
	x::w::text_param description(
		description_format fmt,
		const std::vector<parameter_value> &values
	) const;

	//! Call parse_function_or_generator to parse a function.

	//! If successful, add it to the parsed vector.

	bool try_parse_function(
		const x::xml::readlock &root,
		std::vector<const_appgenerator_function> &parsed) const;

	//! Create a new function with default parameter values.

	appgenerator_function create() const;

private:

	//! Common logic for creating the appropriate generator function.

	appgenerator_function create(std::vector<parameter_value> &&args) const;

public:
	//! Generate the XML for this function, with the given values.
	void save(const x::xml::writelock &parent,
		  const std::vector<parameter_value> &parameter_values,
		  appgenerator_save &info) const;
};

//! Implementation object.

//! \see uicompiler_generators

class uicompiler_generatorsObj::implObj : virtual public x::obj {

public:

	//! All parsed functions.

	std::unordered_multimap<std::string,
				const parse_function> parsed_functions;

	//! Constructor
	implObj();

	//! Destructor
	~implObj();
};

#endif
