/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_rectanglefwd_h
#define x_w_rectanglefwd_h

#include <x/w/types.H>
#include <iostream>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

struct rectangle;
class rectangle_hash;

//! A set of rectangles

typedef std::unordered_set<rectangle, rectangle_hash> rectangle_set;


//! Dump the rectangle into an output stream.

std::ostream &operator<<(std::ostream &, const rectangle &);
LIBCXXW_NAMESPACE_END

#endif
