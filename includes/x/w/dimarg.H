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

//! A dimension specification.

//! Certain parameters may be specified either as a numeric value,
//! that specifies dimensions in millimeters, or as a text string name
//! that references a preset dimension from the default theme.

struct dimarg {

	const std::string_view *string_arg;
	double double_arg;

	//! A string parameter was specified.

	inline dimarg(const std::string_view &string_arg)
		: string_arg(&string_arg)
	{
	}

	//! A numeric value was specified.
	inline dimarg(double v) : string_arg(nullptr), double_arg(v)
	{
	}

	//! Return the value as a string.
	operator std::string() const;
};

LIBCXXW_NAMESPACE_END

#endif
