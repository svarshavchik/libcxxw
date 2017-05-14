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
size_t rows=gridlayout->rows();

size_t cols=gridlayout->cols(2);
\endcode

\c rows() returns the number of rows in the grid.
\c cols() returns the number of cells in the given row. The cell count
does not include spanned cell location in the row. If a cell from an earlier
row spans into this row it does not get counted. If a cell spans multiple
columns on this row it gets counted only once.

Pedantically, \c cols() returns the number of top-left corners of cells in
this row. Cells whose top-left corner are in the given row are number,
consecutively, starting with cell #0.

\code
#include <INSERT_LIBX_NAMESPACE/w/gridfactory.H>

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_row();
INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_columns(4);

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->insert_row(1);

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->insert_columns(1, 2);

\endcode

append_row() adds a new empty row to the grid, and returns a
\ref gridfactory "grid factory". This grid factory adds
elements to the newly-added row.

insert_row() adds a new empty row to the grid before another existing row,
and returns a
\ref gridfactory "grid factory". This grid factory adds
elements to the newly-inserted row.

append_columns() returns a
\ref gridfactory "grid factory" that adds
more elements to an existing row.

insert_columns() returns a
\ref gridfactory "grid factory" that inserts
more elements before existing elements in an existing row.
The factory returned by
\c insert_columns(1, 2) inserts new elements before element #2 (the third one)
in row #1 (the second row in the grid).

\code
gridlayout->remove();
\endcode

remove() removes all existing elements from the grid.

\code
gridlayout->remove(row, col);
\endcode

This remove() removes one element from an existing row in the grid.
Any remaining elements in the row get shifted over by one column, filling
the space from the removed element.

\code
gridlayout->remove_row(2);
\endcode

\c remove_row() removes all elements in an existing row, and the row itself,
from the grid.

\code
elementptr e=gridlayout->get(row, col);
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
