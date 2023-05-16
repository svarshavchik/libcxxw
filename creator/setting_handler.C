/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "creator/setting_handler.H"
#include "creator/appgenerator_save.H"
#include <x/xml/readlock.H>

void setting_handler::define_additional_parameters(
	parameter_value &value,
	std::unordered_map<std::string, x::xml::readlock> &) const
{
}

bool setting_handler::load_from_parent_element(
	const x::xml::readlock &lock,
	const parse_parameterObj &parameter,
	parameter_value &value) const
{
	return false;
}

void setting_handler::load(const setting_load_info &load_info) const
{
	load_info.value.string_value=load_info.lock->get_u32text();
}

bool setting_handler::flag_value() const
{
	return false;
}

void setting_handler::save(const setting_save_info &save_info,
			   appgenerator_save &info) const
{
	if (*save_info.parameter_name.c_str() == '@')
	{
		save_info.lock->attribute({
				save_info.parameter_name.substr(1),
				save_info.value.string_value
			});
		saved_element(save_info, info);
	}
	else
	{
		save_info.lock->create_child()
			->element({save_info.parameter_name})
			->text(save_info.value.string_value)->parent();
		saved_element(save_info, info);
		save_info.lock->get_parent();
	}
}

void setting_handler::saved_element(const setting_save_info &save_info,
				    appgenerator_save &info) const
{
}
