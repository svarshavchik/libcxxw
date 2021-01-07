/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholefwd_H
#define peepholefwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peepholeObj;

//! The working part of a viewport.

//! The peephole is an internal element that's derived from a container.
//! It's set up with its own layout manager, as if it's a container with
//! a single element in it.
//!
//! The internal implementation object:
//!
//! - provides its own metrics: the peephole's size.

typedef ref<peepholeObj> peephole;

//! A nullable pointer reference to a \ref peephole "peephole".

typedef ptr<peepholeObj> peepholeptr;


LIBCXXW_NAMESPACE_END
#endif
