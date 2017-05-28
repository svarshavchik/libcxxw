/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_elementfwd_h
#define peepholed_elementfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class peepholed_elementObj;

//! An element in a peephole.

//! The element in the peephole provides its preferred horizontal and vertical
//! scroll increment.

typedef ref<peepholed_elementObj> peepholed_element;

LIBCXXW_NAMESPACE_END

#endif
