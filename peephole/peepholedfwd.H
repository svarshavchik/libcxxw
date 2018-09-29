/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholedfwd_h
#define peepholedfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peepholedObj;

//! An element in a peephole.

//! The element in the peephole provides its preferred horizontal and vertical
//! scroll increment.

typedef ref<peepholedObj> peepholed;

//! A nullable pointer reference to the peepholed element.

typedef ptr<peepholedObj> peepholedptr;

LIBCXXW_NAMESPACE_END

#endif
