/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef redraw_priority
#define redraw_priority

#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

//! We get better visual results when widgets get redrawn in a particular order
typedef int redraw_priority_t;

//! This widget is expected to draw a clear area
static constexpr redraw_priority_t clear_area=0;

//! Garden-variety widget
static constexpr redraw_priority_t widget_area=1;

LIBCXXW_NAMESPACE_END

#endif
