/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dimarg_h
#define x_w_dimarg_h

#include <x/w/namespace.H>
#include <string>
#include <utility>
#include <string_view>
#include <type_traits>

LIBCXXW_NAMESPACE_START

struct dimarg {

	const std::string_view *string_arg;
	double double_arg;

	inline dimarg(const std::string_view &string_arg)
		: string_arg(&string_arg)
	{
	}

	inline dimarg(double v) : string_arg(nullptr), double_arg(v)
	{
	}

	operator std::string() const;
};

LIBCXXW_NAMESPACE_END

#endif
