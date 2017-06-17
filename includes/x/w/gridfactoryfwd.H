/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridfactoryfwd_h
#define x_w_gridfactoryfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class gridfactoryObj;

/*!
Factory implementation for a grid.

This grid factory gets returned by the grid layout manager. Most of the
methods that create new display elements in the grid are inherited
from \ref factory "the factory base class". Each call to
the layout manager's methods add the new element to the grid, one after
another on the same row.

The \ref gridlayoutmanager "grid layout manager's" methods, like append_row()
and insert_row(),
return a grid factory that add new elements accordingly (to a new row
appended to the grid, a row inserted in the middle of the grid, etc...).

This factory also implements additional methods described below. These
methods modify the position, alignment, or other attributes of the
new element, and must be called before creating the new element. These
methods returns \c this, for convenient chaining, example:

\code
gridfactory->padding(2.5).create_label({"Hello world!"});
\endcode

\par Borders and padding

\code
INSERT_LIBX_NAMESPACE::w::border_infomm new_border;

gridfactory->border(new_border);
gridfactory->left_border(new_border);
gridfactory->right_border(new_border);
gridfactory->top_border(new_border);
gridfactory->bottom_border(new_border);

gridfactory->padding(2.5);
gridfactory->left_padding(2.5);
gridfactory->right_padding(2.5);
gridfactory->top_padding(2.5);
gridfactory->bottom_padding(2.5);
\endcode

Each display element in the grid has borders and inner padding.
Before creating and adding a new display element to the grid, these
methods specify its border and the padding.

border() and padding() set the border and the padding for all four sides,
or they can be set individually.

A border gets specified as a \ref border_infomm "INSERT_LIBX_NAMESPACE::w::border_infomm".
The padding gets specified in millimeters.

\code
gridfactory->border("inputfocuson_border");
gridfactory->left_border("inputfocuson_border");
gridfactory->right_border("inputfocuson_border");
gridfactory->top_border("inputfocuson_border");
gridfactory->bottom_border("inputfocuson_border");
\endcode

A border defined in the current theme can also be specified, alternatively.
This is mostly for the library's internal use.

\par Spanning and alignment

\code
gridfactory->colspan(2);
gridfactory->rowspan(2);
\endcode

By default, each new element added to the grid is fitted in its cell.
colspan() specifies that the new element will span across two or more
cells, as specified. Similarly, rowspan() specifries that the new element
spans two or more rows, as specified.

\code
gridfactory->halign(halign::right);
gridfactory->valign(valign::bottom);
\endcode

The grid layout manager sizes each row and column to fit the largest
element in that row or column. Any other elements in the same row or column
end up with some extra empty space on account of that.

halign() specifies how the element gets horizontally positioned within its
cell. The supported values are:

- \c INSERT_LIBX_NAMESPACE::w::halign::left: positioned flush against the left margin (default).

- \c INSERT_LIBX_NAMESPACE::w::halign::right: positioned flush against the right margin.

- \c INSERT_LIBX_NAMESPACE::w::halign::center: the element is centered in its column.

valign() specifies how the element gets vertically positioned within its
cell. The supported values are:

- \c INSERT_LIBX_NAMESPACE::w::valign::top: positioned flush against the top margin (default).

- \c INSERT_LIBX_NAMESPACE::w::valign::bottom: positioned flush against the bottom margin.

- \c INSERT_LIBX_NAMESPACE::w::valign::middle: the element gets position in the middle of its row.

Additionally, \c INSERT_LIBX_NAMESPACE::w::halign::fill and
\c INSERT_LIBX_NAMESPACE::w::valign::fill are available options which
stretch the display element to the full width or height of its row or column,
if its bigger because of other display elements in the same row or column.
This should be used only with nested \ref container "containers" that are
placed into the grid layout manager.

\code
gridfactory->remove_when_hidden();
\endcode

Specifies that space for the new element gets removed from the grid when
the new element is hidden.

\note
The borders on the opposite sites of a hidden element do not get collapsed.

*/

typedef ref<gridfactoryObj> gridfactory;

LIBCXXW_NAMESPACE_END

#endif
