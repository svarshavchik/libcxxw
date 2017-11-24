/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_border_argfwd_H
#define x_w_border_argfwd_H

#include <x/w/border_infommfwd.H>
#include <string>
#include <variant>

LIBCXXW_NAMESPACE_START

//! A border can be specified as either a border_infomm, or a theme border.

typedef std::variant<border_infomm, std::string> border_arg;

LIBCXXW_NAMESPACE_END

#endif
