/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dimarg_h
#define x_w_dimarg_h

#include <x/w/namespace.H>
#include <string>
#include <variant>

LIBCXXW_NAMESPACE_START

//! A dimension specification.

//! Certain parameters may be specified either as a numeric value,
//! that specifies dimensions in millimeters, or as a text string name
//! that references a preset dimension from the default theme.

typedef std::variant<std::string, double> dim_arg;

LIBCXXW_NAMESPACE_END

#endif
