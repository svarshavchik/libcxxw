/*
** Copyright 2017-2019 Double Precision, Inc.
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
//! display theme (this is mostly for the library's internal use).
//!
//! The numeric value must be positive. The actual number of pixels gets
//! computed based on the current theme's scaling factor. A non-0
//! dimension is always at least one pixel.

typedef std::variant<std::string, double> dim_arg;

LIBCXXW_NAMESPACE_END

#endif
