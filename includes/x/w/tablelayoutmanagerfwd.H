/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanagerfwd_h
#define x_w_tablelayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class tablelayoutmanagerObj;

/*! The table layout manager


*/

typedef ref<tablelayoutmanagerObj> tablelayoutmanager;

//! A constant \ref tablelayoutmanager "table layout manager".

//! \see tablelayoutmanager

typedef const_ref<tablelayoutmanagerObj> const_tablelayoutmanager;

//! A nullable pointer reference to a \ref tablelayoutmanager "table layout manager".

//! \see tablelayoutmanager

typedef ptr<tablelayoutmanagerObj> tablelayoutmanagerptr;

//! A nullable pointer reference to a const \ref tablelayoutmanager "table layout manager".

//! \see tablelayoutmanager

typedef const_ptr<tablelayoutmanagerObj> const_tablelayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
