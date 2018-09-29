/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_listcontainerfwd_h
#define peepholed_listcontainerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peepholed_listcontainerObj;

//! A listcontainer inside a peephole.

//! Overrides the following:
//!
//! - Defines this container's label font as "list", and captures it as
//! the reference font.
//!
//! - The parent container is the peephole. Sets its metrics to be the same
//! width metrics as the list container, and computes the height metrics
//! based on the reference font's height and list item padding, times
//! the number of rows.

typedef ref<peepholed_listcontainerObj> peepholed_listcontainer;

//! A nullable pointer reference to a \ref peepholed_listcontainer "peepholed top level container

typedef ptr<peepholed_listcontainerObj> peepholed_listcontainerptr;

LIBCXXW_NAMESPACE_END

#endif
