/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef gridfactoryfwd_h
#define gridfactoryfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class gridfactoryObj;

//! Factory implementation for a grid.

//! This grid factory gets returned by the grid layout manager's insert()
//! method. When a new element gets created, it gets inserted into the
//! given grid address.

typedef ref<gridfactoryObj> gridfactory;

LIBCXXW_NAMESPACE_END

#endif
