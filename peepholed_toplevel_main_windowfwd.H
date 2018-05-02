/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_main_windowfwd_h
#define peepholed_toplevel_main_windowfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>
#include "peephole/peepholed_toplevelobj.H"

LIBCXXW_NAMESPACE_START

class peepholed_toplevel_main_windowObj;

//! The peepholed element for a normal application window.

//! Implements the top level peepholed element interface for a normal
//! top level application window.
//!
//! \see create_peephole_toplevel

typedef ref<peepholed_toplevel_main_windowObj> peepholed_toplevel_main_window;

//! A nullable pointer reference to a \ref peepholed_toplevel_main_window "peepholed top level container

typedef ptr<peepholed_toplevel_main_windowObj
	    > peepholed_toplevel_main_windowptr;

LIBCXXW_NAMESPACE_END

#endif
