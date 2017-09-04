/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dimarg.H"
#include <x/visitor.H>
#include <sstream>

LIBCXXW_NAMESPACE_START

dimarg::dimarg(const std::string_view &string_arg)
	: superclass_t{string_arg}
{
}

dimarg::dimarg(double v) : superclass_t{v}
{
}

dimarg::operator std::string() const
{
	return std::visit(visitor{
			[](const std::string_view &s) {
				return std::string{s};
			},
			[](const double &d)
			{
				std::ostringstream o;

				o << d;
				return o.str();
			}
		}, static_cast<const superclass_t>(*this));
}

dimarg::~dimarg()=default;

LIBCXXW_NAMESPACE_END
