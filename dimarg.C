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
		return std::string{*string_arg};

	std::ostringstream o;

	o << double_arg;
	return o.str();
}

LIBCXXW_NAMESPACE_END
