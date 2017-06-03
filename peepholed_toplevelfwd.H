/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevelfwd_h
#define peepholed_toplevelfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class peepholed_toplevelObj;

//! An element in a top level peephole.

//! The element in the peephole provides its preferred horizontal and vertical
//! scroll increment, as well as its maximum size.

typedef ref<peepholed_toplevelObj> peepholed_toplevel;

LIBCXXW_NAMESPACE_END

#endif
