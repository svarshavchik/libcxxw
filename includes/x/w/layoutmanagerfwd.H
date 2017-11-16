/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_layoutmanagerfwd_h
#define x_w_layoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class layoutmanagerObj;

//! A layout manager

//! Every container has a layout manager, that manages the elements inside
//! the container element. get_layoutmanager() returns the container's
//! layout manager, which can be:
//!
//! - \ref gridlayoutmanager "The grid layout manager"
//! - \ref listlayoutmanager "The list layout manager"
//! - \ref menubarlayoutmanager "The menu bar layout manager"
//! - \ref standard_comboboxlayoutmanager "The standard combo-box layout manager"
//! - \ref editable_comboboxlayoutmanager "The editable combo-box layout manager"
//!
//! The layout manager returned by get_layoutmanager() typically acquires
//! an internal lock on the container. The lock gets released when all
//! references to the returned layout manager object go out of scope and get
//! destroyed. At this time, any updates to the container are processed and
//! finalized.
//!
//! The internal lock can block the background thread that updates the
//! display. The layout manager object handle returned by get_layoutmanager()
//! should not persist for a long time; otherwise the display will be
//! unresponsive.

typedef ref<layoutmanagerObj> layoutmanager;

//! A constant generic layout manager.

//! \see layoutmanager

typedef const_ref<layoutmanagerObj> const_layoutmanager;

LIBCXXW_NAMESPACE_END

#endif
