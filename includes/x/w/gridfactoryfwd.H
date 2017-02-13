/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridfactoryfwd_h
#define x_w_gridfactoryfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class gridfactoryObj;

//! Factory implementation for a grid.

//! This grid factory gets returned by the grid layout manager.

typedef ref<gridfactoryObj> gridfactory;

LIBCXXW_NAMESPACE_END

#endif
