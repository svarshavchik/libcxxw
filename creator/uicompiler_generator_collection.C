/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "creator/uicompiler_generator_collection.H"
#include "creator/uicompiler_generators.H"
#include "creator/uicompiler_generators_impl.H"
#include "creator/appgenerator_function.H"

#include <x/xml/readlock.H>
#include <x/vector.H>

#include <unordered_map>

LOG_CLASS_INIT(uicompiler_generator_collection);

bool uicompiler_generator_collection::parse_function_or_object_generator(
	const x::xml::readlock &root,
	std::vector<parameter_value> &parameter_values
) const
{
	LOG_DEBUG("Attempting to parse " << get_name());

	bool condition_found=false;

	// If there are no conditions that was easy: condition_found is
	// true. Otherwise we'll check them below.
	std::visit(x::visitor{
			[&](const no_condition &)
			{
				condition_found=true;
			},[&, this](const condition_name_and_value &name_value)
			{
				const auto &[n, v]=name_value;

				LOG_DEBUG("Checking " << get_name()
					  << ", if condition <"
					  << n << ">=" << v);

			},[&, this](const condition_exists exists_node)
			{
				const auto &[n, flag]=exists_node;

				LOG_DEBUG("Checking " << get_name()
					  << ", if condition <"
					  << n
					  << "> exists");
			}
		}, condition);

	// Create a lookup from each parameter, by its name, to its
	// parser.

	std::unordered_map<std::string,
			   std::tuple<const parse_parameter *,
				      parameter_value *>>
		single_parameter_lookup;

	// A setting handler's load_from_parent_element() may inform us that
	// it took care of loading itself. Keep track of these setting handlers
	// here.
	std::unordered_set<std::string> already_loaded_parameters;

	// Also create the same number of parameter_values as there are
	// parsed_parameters.

	parameter_values.resize(parsed_parameters.size());

	prepare_object_member_values(parameter_values);

	// Take the opportunity of iterating over each parsed_parameter to
	// call its load_from_parent_element(). But the main reason for this
	// iteration is to create a single_parameter_lookup by name, by the
	// parameter_name, to the parameter and its corresponding value.
	size_t i=0;

	for (auto &p:parsed_parameters)
	{
		LOG_TRACE("   parameter: " << p->parameter_name);

		auto &value=parameter_values[i++];

		if (!(p->load_from_parent_element(root, value)
		      ? already_loaded_parameters.insert(
			      p->parameter_name).second
		      : single_parameter_lookup.emplace(
			      p->parameter_name,
			      std::tuple{&p, &value}).second))
			throw EXCEPTION("Internal exception: duplicate "
					"parameter definition: "
					<< p->parameter_name);
	}

	bool processed_same_xpath=false;

	// If there's only one parameter and it's the same_xpath, this
	// parameter's value is obtained directly.

	if (single_parameter_lookup.size() == 1)
	{
		// Can only be one parameter to this function, like this.

		auto &[parameter, value] =
			single_parameter_lookup.begin()->second;

		if ((*parameter)->same_xpath)
		{
			if (!(*parameter)->load(root, parameter_values[0]))
			{
				LOG_DEBUG("Parameter "
					  << (*parameter)->get_name()
					  << " rejected");
				return false;
			}

			// No more parameter, if this loop below finds even
			// one element it'll fail.

			single_parameter_lookup.clear();

			LOG_TRACE("Using value in the same xpath");

			processed_same_xpath=true;
		}
	}

	// If we can't parse something at first, stuff it in here.
	std::unordered_map<std::string,
			   x::xml::readlock> unparsed_additional_parameters;

	// Examine the XML elements in the <function>, look them up in the
	// single_parameter_lookup table, save their value, then remove
	// this parameter from single_parameter_lookup.

	for (bool flag=!processed_same_xpath && root->get_first_element_child();
	     flag;
	     flag=root->get_next_element_sibling())
	{
		auto name=root->name();

		LOG_TRACE("Node: <" << name << ">");

		// If this function has a specific condition, check to see if
		// we just found it.

		if (std::visit(
			    x::visitor{
				    [&](const no_condition &)
				    {
					    return false;
				    },
				    [&](const std::tuple<std::string,
					std::string>
					&name_value)
				    {
					    // Condition specified an
					    // element with the given
					    // name has the given value.
					    //
					    // Check if this is so.

					    auto &[n, v]=name_value;

					    if (!(name == n &&
						  root->get_text() == v))
						    return false;

					    condition_found=true;
					    LOG_TRACE("Found condition "
						      << n << "=" << v);
					    return true;

				    },[&](const std::tuple<std::string,
					  bool> exists_node)
				    {
					    auto &[n, has_value]=exists_node;

					    if (name != n)
						    return false;

					    condition_found=true;

					    // If has_value, we need
					    // to parse it, otherwise
					    // we skip this element.
					    LOG_TRACE("Found condition "
						      << n
						      << " exists ");
					    return !has_value;
				    }},
			    condition))
			continue;

		if (already_loaded_parameters.find(name) !=
		    already_loaded_parameters.end())
			continue;

		auto parameter_iter=single_parameter_lookup.find(name);

		// If we do not recognize this parameter, put it into the
		// unparsed_additional_parameters bucket, which we'll check
		// later.

		if (parameter_iter == single_parameter_lookup.end())
		{
			if (!unparsed_additional_parameters.emplace(
				    root->name(), root->clone()).second)
			{
				LOG_DEBUG("Multiple <" <<
					  root->name()
					  << "> cannot be additional"
					  " parameters");
				return false;
			}
			continue;
		}

		// Ok, we found the parameter, load() it, then remove it
		// from the single_parameter_lookup list.

		auto &[parameter, value] = parameter_iter->second;

		if (!(*parameter)->load(root, *value))
		{
			LOG_DEBUG("Parameter " << name << " rejected");
			return false;
		}

		single_parameter_lookup.erase(parameter_iter);
		LOG_TRACE("Found parameter " << name);
	}

	if (!condition_found)
	{
		LOG_DEBUG("Required condition not found");
		return false;
	}

	// Everything for is_object_parameter is optional, otherwise...

	if (!is_parameter())
	{
		// ... we expect to have parsed all parameters.

		for (const auto &p:single_parameter_lookup)
		{
			auto &[parameter, value] = p.second;

			if ((*parameter)->is_optional)
				continue;

			LOG_DEBUG("Some parameters were missing (such as <"
				  << p.first
				  << ">)");
			return false;
		}
	}

	// Ok, now all the parsed parameters can pick up their additional
	// parameters.
	//
	// For example, a layoutmanager type can now process its <config>

	for (size_t i=0; i<parsed_parameters.size(); ++i)
	{
		parsed_parameters.at(i)->define_additional_parameters(
			parameter_values.at(i),
			unparsed_additional_parameters
		);
	}

	// This will pick up something that we could not parse and nobody
	// laid a claim on. This will trip up when any new XML constructs
	// get added, but not handled here.

	if (!unparsed_additional_parameters.empty())
	{
		LOG_DEBUG(
			  ({
				  std::ostringstream o;

				  for (const auto &p
					       : unparsed_additional_parameters)
					  o << "<" << p.first
					    << "> cannot be parsed"
					    << std::endl;

				  o.str();
			  }));
		// Maybe some other function, the condition_found
		// is expected to be false, anyway.
		return false;
	}

	LOG_DEBUG("Found");

	return true;
}


void uicompiler_generator_collection::prepare_object_member_values(
	std::vector<parameter_value> &values
) const
{
	size_t n=values.size();

	if (n != parsed_parameters.size())
		throw EXCEPTION("Internal error: generator function"
				" parameter count mismatch"
				" for object members");

	for (size_t i=0; i<n; ++i)
	{
		if (!parsed_parameters[i]->is_object_parameter)
			continue;
		// We will place the parsed functions here.

		values[i].extra.emplace<std::vector<const_appgenerator_function>
					>();
	}
}
