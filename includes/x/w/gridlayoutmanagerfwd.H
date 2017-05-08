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

/*! The grid layout manager.

The grid layout manager arranges its content elements in a rectangular
grid.

\code
#include <INSERT_LIBX_NAMESPACE/w/gridfactory.H>

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_row();
INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_columns(4);

\endcode

append_row() adds a new empty row to the grid, and returns a
\ref gridfactory "grid factory". This grid factory adds
elements to the newly-added row.

append_columns() returns a
\ref gridfactory "grid factory" that adds
more elements to an existing row.

\code
gridlayout->erase();
\endcode

erase() removes all existing elements from the grid.

\code
gridlayout->erase(x, y);
\endcode

This erase() removes one element from an existing row in the grid.
Any remaining elements in the row get shifted over by one column, filling
the space from the removed element.

\code
elementptr e=gridlayout->get(x, y);
\endcode

get() returns the element at the given grid coordinates. A \c nullptr
display element gets returned if no element exists at the given coordinates.


*/

typedef ref<gridlayoutmanagerObj> gridlayoutmanager;

//! A constant grid layout manager.

//! \see gridlayoutmanager

typedef const_ref<gridlayoutmanagerObj> const_gridlayoutmanager;

LIBCXXW_NAMESPACE_END

#endif
