/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_border_argfwd_H
#define x_w_border_argfwd_H

#include <x/w/border_infommfwd.H>
#include <string>
#include <variant>

LIBCXXW_NAMESPACE_START

//! A border that's specified as either a border_infomm, or a theme border.

//! Specifies the visual appearance of a border, as an explicit
//! \ref border_infomm "border_infomm", or a label referencing a border
//! from the current display theme.

typedef std::variant<border_infomm, std::string> border_arg;

LIBCXXW_NAMESPACE_END

#endif
