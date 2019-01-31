/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peephole_toplevel_gridlayoutmanagerfwd_h
#define peephole_toplevel_gridlayoutmanagerfwd_h

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peephole_toplevel_gridlayoutmanagerObj;

//! Grid layout manager for the top level display element.

//! A subclass of the gridlayoutmanager implementation object that's created
//! for the top level display element.
//!
//! This overrides the grid layout manager's recalculate().
//! A _NET_FRAME_EXTENTS update results in update_frame_extents() invoking
//! needs_recalculation(). Override it, get the peephole layout manager, and
//! also trigger its recalculation, where the size of the maximum peephole
//! size gets computed.
//!
//! \see create_peephole_toplevel

typedef ref<peephole_toplevel_gridlayoutmanagerObj
	    > peephole_toplevel_gridlayoutmanager;

LIBCXXW_NAMESPACE_END

#endif
