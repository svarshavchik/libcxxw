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

//! This is the interface specification for an element in a top level peephole.
//!
//! The top level display element, such as the main application window
//! is a container that uses the grid layout manager. The grid consists of
//! a peephole, and its associated scrollbars. The element in the peephole
//! containts the real contents of the top level display element.
//!
//! This is an additional subclass of a peephole element that, in addition
//! to specifying its preferred scrolling increments, defines the maximum
//! size of the peephole.
//!
//! \see create_peephole_toplevel

typedef ref<peepholed_toplevelObj> peepholed_toplevel;

LIBCXXW_NAMESPACE_END

#endif
