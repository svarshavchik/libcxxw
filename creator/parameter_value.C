/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "creator/parameter_value.H"
#include "creator/appgenerator_function.H"
#include "creator/appgenerator_functions.H"
#include "creator/appgenerator_save.H"
#include <x/visitor.H>

void parameter_value::cloned()
{
	std::visit(x::visitor{
			[](std::monostate) {},
			[](existing_appgenerator_functions &generator_info)
			{
				for (auto &f:generator_info.parsed_functions)
				{
					f=f->clone();
				}
			},
			[](std::vector<const_appgenerator_function> &functions)
			{
				for (auto &f:functions)
					f=f->clone();
			}
		}, extra);
}
