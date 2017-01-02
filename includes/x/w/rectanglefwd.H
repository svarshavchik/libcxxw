/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_rectanglefwd_h
#define x_w_rectanglefwd_h

#include <x/w/types.H>
#include <iostream>

LIBCXXW_NAMESPACE_START

struct rectangle;

//! Dump the rectangle into an output stream.

std::ostream &operator<<(std::ostream &, const rectangle &);
LIBCXXW_NAMESPACE_END

#endif
