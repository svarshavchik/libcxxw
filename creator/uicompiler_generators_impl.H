/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef creator_uicompiler_generators_impl_h
#define creator_uicompiler_generators_impl_h

#include "creator/uicompiler_generators.H"
#include "x/w/text_param.H"
#include <x/obj.H>
#include <x/logger.H>
#include <x/visitor.H>
#include <unordered_map>
#include <string>
#include <x/xml/readlock.H>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <tuple>

struct setting_handler;

//! Parsed \ref "uicompiler_generators" information

//! Internal data, parsed contents of a <member> specified in uicompiler.xml

struct parse_member {

	//! The <name> of this member.
	std::string member_name;

	//! Its <type> information.
	const setting_handler *handler;

	//! Parse a <member> declaration.

	//! If there's a <lookup> with an extra <parameter>, this gets
	//! dropped into the extra_members.
	parse_member(const x::xml::readlock &root,
		     std::vector<parse_member> &extra_members);

	//! Construct an extra single_value member referenced in a <lookup>
	parse_member(const std::string &extra_single_value);

};

//! Parsed \ref "uicompiler_generators" information

//! Internal data, parsed contents of a <parameter> specified in uicompiler.xml

struct parse_parameter {

	//! The parameter's <name>
	std::string parameter_name;

	//! The parameter's <type>
	const setting_handler *handler=nullptr;

	//! This parameter specifies <xpath>.</xpath>
	bool same_xpath=false;

	//! This parameter has an <object>
	bool is_object_parameter=false;

	//! This parameter has a <scalar>
	bool scalar_parameter=false;

	//! All of this parameter's <member>s.
	std::vector<parse_member> object_members;

	//! Parse a <parameter> declaration.

	//! If there's a <lookup> with an extra <parameter>, this gets
	//! dropped into the extra_members.
	parse_parameter(const x::xml::readlock &root,
			std::vector<parse_parameter> &extra_parameters);

	//! Construct an extra single_value member referenced in a <lookup>
	parse_parameter(const std::string &extra_single_value);

	//! Generate the XML for this parameter.

	//! Writes this parameter's XML as a child of the given write lock's
	//! node, and leavfes the write lock on the same node.
	void save(const x::xml::writelock &lock,
		  const std::u32string &value) const;
};

//! Parsed \ref "uicompiler_generators" information

//! Internal data, parsed contents of a <function> specified in uicompiler.xml

struct parse_function {

	LOG_CLASS_SCOPE;

	//! The parameter's name.

	std::string name;

	//! This parameter does not have a <condition>
	typedef std::monostate no_condition;

	//! This <condition> has a <name> and a <value>
	typedef std::tuple<std::string, std::string> condition_name_and_value;

	//! This condition> has an <exists> value.

	//! The bool flag indicates whether the given string exists as a
	//! discrete parameter. It's possible that <exists> names one of the
	//! parameters, or a different element that exists by itself.
	typedef std::tuple<std::string, bool> condition_exists;

	//! Whether or not this function has a <condition>
	std::variant<no_condition,
		     condition_name_and_value,
		     std::tuple<std::string, bool>> condition;

	//! This <function> is a <new_element> or <new_container>.

	bool new_element_or_container=false;

	//! All of this function's <parameter>s.
	std::vector<parse_parameter> parsed_parameters;

	//! Parse this <function> in uicompiler.xml
	parse_function(const x::xml::readlock &root);

	//! Give a description of tihs function.
	x::w::text_param description(description_format fmt) const;

	//! Try to parse this function.

	//! For an element in the theme file that specifies this function's
	//! name try to parse it. There can be several parsed function's
	//! with the given name, if they specify <condition>s.
	//!
	//! Returns true if the function was parsed and added to the "funcs"
	//! parameter. Returns false if the function's conditions are not met.

	bool parse_generator(const x::xml::readlock &root,
			     std::vector<appgenerator_function> &funcs)
		const;

	//! Create a new function with default parameter values.

	appgenerator_function create() const;

private:

	//! Common logic for creating the appropriate generator function.

	appgenerator_function create(std::vector<std::u32string> &&args) const;

public:
	//! Generate the XML for this function, with the given values.
	void save(const x::xml::writelock &parent,
		  const std::vector<std::u32string> &parameter_values) const;
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
