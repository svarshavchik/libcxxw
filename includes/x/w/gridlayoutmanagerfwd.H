/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanagerfwd_h
#define x_w_gridlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN new_gridlayoutmanager;
class LIBCXX_PUBLIC gridlayoutmanagerObj;

/*! The grid layout manager.

\code
#include <INSERT_LIBX_NAMESPACE/w/gridlayoutmanager.H>

INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager nglm;

factory->create_container(
    [&]
    (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
    {
        INSERT_LIBX_NAMESPACE::w::gridlayoutmanager pane_lm=c->get_layoutmanager();
    },
    nglm);
\endcode

Passing an
\ref new_gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager"
to create_container() creates a new container that uses the
grid layout manager.
The grid layout manager arranges its content elements in a rectangular
grid.

Only one execution thread at a time has access to the underlying container using
the grid layout manager. The grid layout manager object holds an internal
lock on the container, blocking other execution threads from accessing the
container until the grid layout manager object goes out of scope and gets
destroyed.

\code
size_t rows=gridlayout->rows();

size_t cols=gridlayout->cols(2);
\endcode

\c rows() returns the number of rows in the grid.
\c cols() returns the number of cells in the given row. The cell count
does not include cells that span that row. A cell from an earlier
row that spans into this row does not get included in the count.
A cell in the row spans multiple columns in this row gets counted only once.

Pedantically, \c cols() returns how many cells have their top-left ocrner
in this row. Cells whose top-left corner are in the given row get numbered,
consecutively, starting with cell #0.

\code
#include <INSERT_LIBX_NAMESPACE/w/gridfactory.H>

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_row();
INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->append_columns(4);

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->insert_row(1);

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->replace_row(1);

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->insert_columns(1, 2);

INSERT_LIBX_NAMESPACE::w::gridfactory factory=gridlayout->replace_cell(1, 2);

\endcode

append_row() adds a new empty row to the grid, and returns a
\ref gridfactory "grid factory". This grid factory adds
elements to the newly-added row.

insert_row() adds a new empty row to the grid before another existing row,
and returns a
\ref gridfactory "grid factory". This grid factory adds
elements to the newly-inserted row.

replace_row() removes all elements from an existing row
and returns a
\ref gridfactory "grid factory". This grid factory adds
replacement elements in the row.

append_columns() returns a
\ref gridfactory "grid factory" that adds
more elements to an existing row.

insert_columns() returns a
\ref gridfactory "grid factory" that inserts
more elements before existing elements in an existing row.
The factory returned by
\c insert_columns(1, 2) inserts new elements before element #2 (the third one)
in row #1 (the second row in the grid).

replace_cell() returns a
\ref gridfactory "grid factory" that replaces the specified element.
The factory returned by
\c replace_cell(1, 2) replaces element #2 (the third one)
in row #1 (the second row in the grid). Using this factory to create an
element replaces this existing element. Creating more elements using the
same factory results in only the last created element replacing the existing
element.

\code
gridlayout->remove();
\endcode

remove() removes all existing elements from the grid. Also resets all
default row and column defaults (borders, paddings, percentages).

\code
gridlayout->remove(row, col);
\endcode

This remove() removes one element from an existing row in the grid.
Any remaining elements in the row get shifted over by one column, filling
the space from the removed element.

\code
gridlayout->remove_row(2);
gridlayout->remove_rows(2, 3);
\endcode

\c remove_row() removes all elements in an existing row, and the row itself,
from the grid.

The second parameter to \c remove_rows() specifies the nmber of rows to
remove starting with the row specified by the first parameter.

\code
elementptr e=gridlayout->get(row, col);
\endcode

get() returns the element at the given grid coordinates. A \c nullptr
widget gets returned if no element exists at the given coordinates.

\par Resorting rows

\code
std::vector<size_t> index;

l->resort_rows(index);
\endcode

resort_rows() moves the rows in the grid. The index vector's size must
match the number of rows in the grid. The index vector's values must consist
of strictly increasing values, starting at 0, in some particular order,
meaning that: for a grid of 8 rows, the index vector must contain values
0 through 7.

resort_rows() reorders the rows as if the index vector gets
sorted in its natural order, and the corresponding rows get
repositioned accordingly. For example, when the index vector contains
the values:

\code
std::vector<size_t> index={6, 1, 2, 0, 4, 5, 3, 7};

l->resort_rows(index);
\endcode

This ends up moving the cells in the first row, row \#0 to row \#6,
row \#6 to \#3, and row \#3 to row \#0.

The results are unspecified if the index vector fails to meet the stated
requirements.

\note All \ref focusable "focusable" widgets in the resorted rows remain
in the same tabbing order.

\par Row and column defaults

\code
INSERT_LIBX_NAMESPACE::w::border_infomm new_border;

gridlayout->default_row_border(0, new_border);
gridlayout->default_row_border(0, "thin_0%");
gridlayout->requested_row_height(0, 100);
gridlayout->row_alignment(0, INSERT_LIBX_NAMESPACE::w::valign::middle);
gridlayout->default_top_padding(0, 2.0);
gridlayout->default_bottom_padding(0, 2.0);

gridlayout->default_col_border(0, new_border);
gridlayout->default_col_border(0, "thin_0%");
gridlayout->requested_col_width(0, 100);
gridlayout->col_alignment(0, INSERT_LIBX_NAMESPACE::w::halign::center);
gridlayout->default_left_padding(0, 2.0);
gridlayout->default_right_padding(0, 2.0);

gridlayout->remove_row_defaults(0);
gridlayout->remove_col_defaults(0);
\endcode

These methods define the default properties of rows and columns. Rows and
columns are numbered, starting with row and column 0 in the top-left corner.

default_row_border() sets the default border drawn above the given row.
default_col_border() sets the default border drawn on the left side of the
given column. \ref border_infomm "INSERT_LIBX_NAMESPACE::w::border_infomm"
specifies the border; alternatively the current theme's border gets specified
by name.

requested_row_height() and requested_col_width() sets the desired size of
the row or the column, specified as a percentage between 0-100 of the
overall grid's width or height. This is just a hint to the grid layout
manager. If the grid's size exceeds its standard size, the additional real
estate gets doled out to the rows and the columns according to their requested
sizes.

row_alignment() and col_alignment() specify the default alignment for the
cells in the grid.

default_top_padding(), default_bottom_padding(), default_left_padding() and
default_right_padding() set the default padding for the specified row or
column.

\note

row_alignment() and col_alignment() gets set before creating the
cells in the given row or column. Once created, each individual cell's
alignment gets fixed, and does not get affected by remove_row_defaults() and
remove_col_defaults().

row_alignment() must get set before using append_row() or insert_row()
to construct a factory for cells in the row.

\par Populating from an XML-based specification

\code
gridlayout->set_background_color(INSERT_LIBX_NAMESPACE::w::white);
gridlayout->remove_background_color();

gridlayout->generate("main-layout", generators, elements);
\endcode

set_background_color() is equivalent to invoking set_background_color()
and remove_background_color() on the grid layout manager's underlying
container widget. This is mostly for use by an XML-based
specification of the container's contents. generate() creates
new contents from the
loaded theme file. See the tutorial for more information.

*/

typedef ref<gridlayoutmanagerObj> gridlayoutmanager;

//! A constant grid layout manager.

//! \see gridlayoutmanager

typedef const_ref<gridlayoutmanagerObj> const_gridlayoutmanager;

//! A nullable pointer reference to a \ref gridlayoutmanager "grid layout manager".

typedef ptr<gridlayoutmanagerObj> gridlayoutmanagerptr;

//! A nullable pointer reference to a constant \ref gridlayoutmanager "grid layout manager".

//! \see gridlayoutmanager

typedef const_ptr<gridlayoutmanagerObj> const_gridlayoutmanagerptr;

class LIBCXX_HIDDEN new_gridlayoutmanager;

LIBCXXW_NAMESPACE_END

#endif
