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

typedef ref<gridlayoutmanagerObj> gridlayoutmanager;

//! A constant grid layout manager.

//! \see gridlayoutmanager

typedef const_ref<gridlayoutmanagerObj> const_gridlayoutmanager;

LIBCXXW_NAMESPACE_END

#endif
