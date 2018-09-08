/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_synchronized_axisfwd_h
#define x_w_table_synchronized_axisfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include "x/w/tablelayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

/*! Override synchronized_axisObj::implObj for tables.

Overrides the synchronization implementation object for containers that use
the \ref tablelayoutmanager "table layout manager", which is mostly implemented
using the list layout manager.

*/

typedef ref<tablelayoutmanagerObj::table_synchronized_axisObj> table_synchronized_axis;

//! A constant \ref table_synchronized_axis "table synchronization implementation".

//! \see table_synchronized_axis

typedef const_ref<tablelayoutmanagerObj::table_synchronized_axisObj> const_table_synchronized_axis;

//! A nullable pointer reference to a \ref table_synchronized_axis "table synchronization implementation".

//! \see table_synchronized_axis

typedef ptr<tablelayoutmanagerObj::table_synchronized_axisObj> table_synchronized_axisptr;

//! A nullable pointer reference to a const \ref table_synchronized_axis "table synchronization implementation".

//! \see table_synchronized_axis

typedef const_ptr<tablelayoutmanagerObj::table_synchronized_axisObj> const_table_synchronized_axisptr;

LIBCXXW_NAMESPACE_END

#endif
