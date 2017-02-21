/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanagerfwd_h
#define x_w_gridlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class gridlayoutmanagerObj;

//! The grid layout manager.

//! The grid layout manager arranges its content elements in a rectangular
//! grid.
//!
//! \code
//! #include <INSERT_LIBX_NAMESPACE/w/gridfactory.H>
//!
//! INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_row();
//!
//! \endcode
//!
//! append_row() adds a new empty row to the grid, and returns a
//! \ref gridfactory "grid factory". The grid factory is set up to add
//! elements to the newly-added row.

typedef ref<gridlayoutmanagerObj> gridlayoutmanager;

//! A constant grid layout manager.

//! \see gridlayoutmanager

typedef const_ref<gridlayoutmanagerObj> const_gridlayoutmanager;

LIBCXXW_NAMESPACE_END

#endif
