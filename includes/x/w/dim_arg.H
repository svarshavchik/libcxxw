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

//! Certain parameters may be specified either as a numeric value, or as
//! a text string. A numerical value specifies a size expressed in
//! millimeters. A text string references a size specified by the current
//! display theme. This is mostly for the library's internal use.

typedef std::variant<std::string, double> dim_arg;

LIBCXXW_NAMESPACE_END

#endif
