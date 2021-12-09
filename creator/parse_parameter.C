/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "creator/parse_parameter.H"
#include "creator/setting_handler.H"
#include "creator/object_appgenerator_function.H"
#include <x/exception.H>
#include <x/xml/readlock.H>

#include <string>
#include <optional>

parse_parameterObj::parse_parameterObj()=default;

bool parse_parameterObj::load_from_parent_element(const x::xml::readlock &lock,
						  parameter_value &value) const
{
	if (!handler)
		return false;

	return handler->load_from_parent_element(lock, *this, value);
}

bool parse_parameterObj::is_parameter() const
{
	return true;
}

std::string parse_parameterObj::get_name() const
{
	return parameter_name;
}

void parse_parameterObj
::define_additional_parameters(parameter_value &value,
			       std::unordered_map<std::string,
			       x::xml::readlock>
			       &unparsed_additional_parameters) const
{
	if (is_object_parameter)
		return;

	handler->define_additional_parameters(value,
					      unparsed_additional_parameters);
}

//! Extract additional information from a <lookup>.

//! This parses the <lookup> specification in uicompiler.xml. This tells
//! us what kind of a parameter this is.

struct lookup_info {

	//! The lookup function
	std::string function;

	//! An extra single_value parameter
	std::string extra_single_value;

	//! Constructor
	lookup_info(const x::xml::const_readlock &lookup);
};

lookup_info::lookup_info(const x::xml::const_readlock &lookup)
{
	auto root=lookup->clone();

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto name=root->name();

		if (name == "function")
		{
			function=root->get_text();
			continue;
		}
		if (name == "default_params")
			continue;
		if (name == "modify")
			continue;
		if (name == "prepend-parameter")
			continue;

		if (name != "parameter")
			throw EXCEPTION("Unknown node in <lookup>: <"
					<< name << ">");

		auto parameter=root->get_text();

		if (parameter == "lock")
			continue;

		if (parameter.substr(0, 20) == "single_value(lock, \"")
		{
			parameter=parameter.substr(20);

			extra_single_value=std::string{parameter.begin(),
				std::find(parameter.begin(),
					  parameter.end(),
					  '"')};
			continue;
		}

		throw EXCEPTION("Unknown <parameter> in <lookup>: "
				<< parameter);
	}
}

////////////////////////////////////////////////////////////////////////////
//
// Parse a <member> specification.

void parse_parameterObj::define_member(const x::xml::readlock &root,
				    std::vector<parse_parameter> &extra_members)
{
	std::string member_type_name;

	std::optional<lookup_info> lookup;

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto member_prop=root->clone();

		auto name=member_prop->name();

		if (name == "name")
		{
			parameter_name=member_prop->get_text();
			continue;
		}
		if (name == "type")
		{
			member_type_name=member_prop->get_text();
			continue;
		}

		if (name == "field")
			continue;

		if (name == "lookup")
		{
			lookup.emplace(root);
			continue;
		}
		if (name == "method_call")
		{
			member_type_name="method_call";
			continue;
		}
		throw EXCEPTION("Unknown <member> node: " << name);
	}

	// We know what to do with a <lookup> only for a single_value.
	if (lookup)
	{
		if (member_type_name != "single_value")
		{
			throw EXCEPTION("Internal error: lookup is for"
					" something other than a single_value");
		}
	}

	handler_name=member_type_name;

	if (member_type_name == "single_value")
	{
		if (lookup)
		{
			// Replace the type with the lookup function.

			if (lookup->function.substr(0, 13)
			    == "compiler.get_" &&
			    lookup->function.size() > 29 &&
			    lookup->function.substr(lookup->function.size()-16)
			    == "_appearance_base")
			{
				handler_name=lookup->function.substr(
					13,
					lookup->function.size()-29
				);

				member_type_name="lookup_appearance_base";
			}
			else if (lookup->function.substr(0, 27) ==
			    "compiler.lookup_appearance<" &&
				 lookup->function.size() > 28)
			{
				handler_name=lookup->function.substr(
					27,
					lookup->function.size()-28
				);
				member_type_name="lookup_appearance";
			}
			else
			{
				member_type_name=lookup->function;
				handler_name=member_type_name;
			}
			if (!lookup->extra_single_value.empty())
			{
				throw EXCEPTION("Not implemented");
			}
		}
	}

	auto iter=setting_handler::member_types.find(member_type_name.c_str());

	if (iter == setting_handler::member_types.end())
	{
		throw EXCEPTION("Unknown <member> <type> "
				<< member_type_name
				<< " (of " << parameter_name << ")");
	}

	handler=iter->second;
}

void parse_parameterObj::define_parameter(const x::xml::readlock &root,
				       std::vector<parse_parameter>
				       &extra_parameters)
{
	std::string parameter_type;
	std::string factory_wrapper;
	std::optional<lookup_info> lookup;

	parsed_parameters.reserve(root->get_xpath("member")->count()+
				  root->get_xpath("member/lookup/parameter")
				  ->count());

	std::unordered_set<std::string> all_parsed_members;

	std::vector<parse_parameter> extra_members;

	for (bool flag=root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto parameter_prop=root->clone();

		auto name=parameter_prop->name();

		if (name == "name")
		{
			parameter_name=parameter_prop->get_text();
			continue;
		}
		if (name == "type")
		{
			parameter_type=parameter_prop->get_text();
			continue;
		}

		if (name == "lookup")
		{
			lookup.emplace(parameter_prop);
			continue;
		}

		if (name == "xpath")
		{
			auto xpath=parameter_prop->get_text();
			if (xpath != ".")
			{
				std::cerr << "Unknown <xpath> "
					"value: "
					  << xpath
					  << std::endl;
				continue;
			}
			same_xpath=true;
			continue;
		}

		if (name == "factory_wrapper")
		{
			factory_wrapper=
				parameter_prop->get_text();
			continue;
		}

		if (name == "scalar")
		{
			scalar_parameter=true;
			break;
		}

		if (name == "object")
		{
			is_object_parameter=true;
			continue;
		}

		if (name == "default_constructor_params")
		{
			is_optional=true;
			continue;
		}

		if (name == "member_name" ||
		    name == "before-passing-parameter" ||
		    name == "after-passing-parameter" ||
		    name == "initialize_self" ||
		    name == "method_call")
			continue;

		if (name == "member")
		{
			auto new_member=parse_parameter::create();

			new_member->define_member(parameter_prop,
						  extra_members);

			if (!all_parsed_members
			    .insert(new_member->parameter_name).second)
				throw EXCEPTION("Duplicate member: "
						<< new_member->parameter_name);

			parsed_parameters.push_back(new_member);

			continue;
		}
		throw EXCEPTION("Unknown <parameter> node: "
				<< name);
	}

	if (parsed_parameters.empty())
		is_object_parameter=false;

	// Review the extra members we collected

	// Nothing more needs to be done if a regular member with that name
	// was specified, this directive will piggy-back on top of it.
	//
	// Otherwise we add it as an additional member.

	for (auto &m:extra_members)
	{
		if (all_parsed_members.find(m->parameter_name) !=
		    all_parsed_members.end())
		{
			continue;
		}

		parsed_parameters.push_back(m);
	}

	parsed_parameters.shrink_to_fit();

	for (const auto &p:parsed_parameters)
	{
		if (!p->handler)
			throw EXCEPTION("Internal error: handler for "
					<< p->parameter_name
					<< " was not set in define_parameter");

		parsed_parameter_functions.emplace(
			p->parameter_name,
			x::ref<object_appgenerator_functionObj>::create(
				get_name(),
				p,

				// If this is a checkbox we'll initialize the
				// default value as checked.
				parameter_value{
					p->handler->flag_value() ? U"1":U""
				}));
	}

	if (scalar_parameter)
	{
		if (is_object_parameter ||
		    !parsed_parameters.empty())
			throw EXCEPTION("<scalar> together with "
					"other attributes");
		return;
	}

	if (is_object_parameter)
	{
		is_optional=true;
		return;
	}

	if (!parsed_parameters.empty())
		throw EXCEPTION("<member> specified without an "
				"<object>");

	// We know what to do with a <lookup> only for a single_value.

	if (lookup)
	{
		if (parameter_type != "single_value")
		{
			throw EXCEPTION("Internal error: lookup is for"
					" something other than a single_value");
		}
	}

	if (parameter_type == "single_value_exists" ||
	    parameter_type == "compiler.shortcut_value")
		is_optional=true;

	handler_name=parameter_type;

	if (parameter_type == "single_value")
	{
		if (lookup)
		{
			// Replace the type with the lookup function.

			if (lookup->function.substr(0, 33)
			    == "compiler.lookup_appearance<const_" &&
			    lookup->function.substr(
				    lookup->function.size()-12)
			    == "_appearance>")
			{
				parameter_type="lookup_appearance";

				handler_name=lookup->function.substr(
					33,
					lookup->function.size()-45
				);
			}
			else
			{
				parameter_type=lookup->function;

				handler_name=parameter_type;

				if (!lookup->extra_single_value.empty())
				{
					// The theme file is going to
					// specify this value, instead.

					extra_parameters.emplace_back(
						parse_parameter::create(
							lookup->
							extra_single_value
						)
					);
				}
			}
		}
	}

	if (parameter_type.substr(0, 16) == "compiler.lookup_" &&
	    parameter_type.size() >= 34 &&
	    parameter_type.substr(parameter_type.size()-18) ==
	    "factory_generators")
	{
		// Use lookup_factory for this, and extra the factory type
		// from the function name.

		parameter_type="lookup_factory";
		handler_name=handler_name.substr(16, handler_name.size()-34);

		if (handler_name.empty())
			// compiler.lookup_factory_generators maps to
			// a "factory".
			handler_name="factory";

	}

	auto iter=setting_handler::parameter_types.find(parameter_type.c_str());

	if (iter == setting_handler::parameter_types.end())
	{
		throw EXCEPTION("Unknown <parameter> <type> "
				<< parameter_type
				<< " (of " << parameter_name << ")");
	}

	if (parameter_name.empty())
		throw EXCEPTION("Parameter <name> not specified");

	handler=iter->second;
}

bool parse_parameterObj::load(const x::xml::readlock &lock,
			      parameter_value &value) const
{
	if (is_object_parameter)
	{
		// Load the parameter value as a list of parsed
		// appgenerator_functions.

		auto &loaded_parameters=
			std::get<std::vector<const_appgenerator_function>>(
				value.extra
			);

		loaded_parameters.reserve(lock->get_child_element_count());

		auto copy=lock->clone();

		// Go through the XML contents, and load one by one.

		for (bool flag=copy->get_first_element_child(); flag;
		     flag=copy->get_next_element_sibling())
		{
			auto name=copy->name();
			LOG_TRACE("Node <" << name << ">");

			auto iter=parsed_parameter_functions.find(name);

			if (iter == parsed_parameter_functions.end())
				return false; // Is not us.

			auto f=iter->second->clone_me();

			if (!f->member->load(copy, *f->value))
				return false;
			loaded_parameters.push_back(f);
		}

		return true;
	}
	if (!handler)
		throw EXCEPTION("Internal error: handler is not set "
				"(" << parameter_name << ")");

	handler->load(lock->clone(), value);
	return true;
}

void parse_parameterObj::save(const x::xml::writelock &lock,
			      const parameter_value &value,
			      bool is_member,
			      appgenerator_save &info) const
{
	if (is_object_parameter)
	{
		lock->create_child()->element({parameter_name});

		auto &values=std::get<std::vector<const_appgenerator_function>>(
			value.extra
		);

		for (const auto &f:values)
			f->save(lock, info);

		if (lock->get_child_element_count() == 0)
		{
			lock->remove();
		}
		else
		{
			lock->get_parent();
		}
		return;
	}

	if (!handler)
		throw EXCEPTION("Internal error: null handler in save()");

	if (same_xpath)
	{
		if (!value.string_value.empty())
			lock->create_child()->text(value.string_value)
				->parent();
		handler->saved_element(
			{lock, parameter_name, value, handler_name,
			is_member}, info);
	}
	else
	{
		if (is_optional && value.string_value.empty())
			return;

		handler->save({lock, parameter_name, value, handler_name,
				is_member},
			info);
	}

	if (scalar_parameter)
	{
		throw EXCEPTION("Unimplemented: scalar");
	}

	if (parsed_parameters.size())
	{
		throw EXCEPTION("Unimplemented: members");
	}
}
