/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_rectanglefwd_h
#define x_w_rectanglefwd_h

#include <x/w/types.H>
#include <iostream>
#include <vector>

LIBCXXW_NAMESPACE_START

struct rectangle;
class rectangle_hash;

//! An area defined by a list of rectangles.

typedef std::vector<rectangle> rectarea;

//! Dump the rectangle into an output stream.

std::ostream &operator<<(std::ostream &, const rectangle &);
LIBCXXW_NAMESPACE_END

#endif
