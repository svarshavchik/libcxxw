/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_parse_parameterobj_h
#define x_w_parse_parameterobj_h

#include "creator/parse_parameterfwd.H"
#include "creator/uicompiler_generator_collection.H"
#include "creator/appgenerator_savefwd.H"
#include <x/xml/readlockfwd.H>
#include <x/xml/writelockfwd.H>
#include <string>
#include <unordered_map>

//! Implementation object.

//! \see parse_parameter

class parse_parameterObj: virtual public x::obj,
			  public uicompiler_generator_collection {

public:

	//! The parameter's <name>
	std::string parameter_name;

	//! The parameter's <type>
	const setting_handler *handler=nullptr;

	//! Additional parameter value for the handler

	//! There's one handler for all lookup*factory types. This tracks
	//! which one it is
	std::string handler_name;

	//! This parameter specifies <xpath>.</xpath>
	bool same_xpath=false;

	//! This parameter has an <object>
	bool is_object_parameter=false;

	//! This parameter has a <scalar>
	bool scalar_parameter=false;

	//! This parameter is optional

	//! Optional parameter

	//! Used for <elements> and <tooltip>, and for <object> <member>s.

	bool is_optional=false;

	//! Constructor

	parse_parameterObj();

	//! Implement is_parameter().

	bool is_parameter() const override;

	//! Implement get_name().
	std::string get_name() const override;

	//! Parse a <parameter> declaration.

	//! Parses a <parameter> declaration in uicompiler.xml
	//!
	//! If there's a <lookup> for a <parameter>, this gets
	//! dropped into the extra_parameters.
	void define_parameter(const x::xml::readlock &root,
			      std::vector<parse_parameter> &extra_parameters);

	//! Parse a <member> declaration.

	//! Parses a <member> declaration in uicompiler.xml. This is handled
	//! like a parameter.
	//!
	//! If there's a <lookup> with an extra <parameter>, this gets
	//! dropped into the extra_members.
	void define_member(const x::xml::readlock &root,
			   std::vector<parse_parameter> &extra_members);


	//! Find any additional parameters this generator needs

	//! After parsing the generators' parameters, all unrecognized
	//! XML entities get collected into the unparsed_additional_parameters,
	//! and each parameter gets an opportunity to claim them.

	void define_additional_parameters(parameter_value &value,
					  std::unordered_map<std::string,
					  x::xml::readlock>
					  &unparsed_additional_parameters)
		const;

	//! Construct an extra single_value member referenced in a <lookup>
	parse_parameterObj(const std::string &extra_single_value);

	//! Load the parameter's value from the parent element

	//! Gets forwarded to handler's

	bool load_from_parent_element(
		const x::xml::readlock &lock,
		parameter_value &value) const;

	//! Load the parameter value

	//! Gets forwarded to handler's load()

	bool load(const x::xml::readlock &lock,
		  parameter_value &value) const;

	//! Generate the XML for this parameter.

	//! Writes this parameter's XML as a child of the given write lock's
	//! node, and leaves the write lock on the same node.
	void save(const x::xml::writelock &lock,
		  const parameter_value &value,
		  bool is_member,
		  appgenerator_save &info) const;

};

#endif
