/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dimarg.H"
#include <sstream>

LIBCXXW_NAMESPACE_START

dimarg::operator std::string() const
{
	if (string_arg)
		return {string_arg->begin(), string_arg->end()};

	// TODO: gcc 6.3.1 incomplete string_view support?

	std::ostringstream o;

	o << double_arg;
	return o.str();
}

LIBCXXW_NAMESPACE_END
