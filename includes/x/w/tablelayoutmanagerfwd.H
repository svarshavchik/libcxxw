/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanagerfwd_h
#define x_w_tablelayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN new_tablelayoutmanager;
class LIBCXX_PUBLIC tablelayoutmanagerObj;

/*! The table layout manager


\code
INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager new_table{
    {
      []
      (const INSERT_LIBX_NAMESPACE::w::factory &f)
      {
          f->create_label("Column 1")->show();
      },
      []
      (const INSERT_LIBX_NAMESPACE::w::factory &f)
      {
          f->create_label("Column 2")->show();
      }
    }
};

INSERT_LIBX_NAMESPACE::w::focusable_container list=f->create_focusable_container(
   []
   (const INSERT_LIBX_NAMESPACE::w::focusable_container &c)
   {
        INSERT_LIBX_NAMESPACE::w::tablelayoutmanager tbl=c->get_layoutmanager();
   },
   new_table);
\endcode

A subclass of the \ref listlayoutmanager "list layout manager" that adds
a header row with optional draggable borders. If adjustments are enabled,
putting the pointer on top of a border between two columns allows it to
be clicked and dragged, adjusting the relative sizes of the columns on
either side of the border.

\ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager"
inherits most configuration settings from the
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager".

\par Table width

\code
new_table.minimum_column_widths={
    {0, 20},
    {1, 20}};

new_table.width=60;
new_table.maximum_table_width=80;

new_table.unlimited_table_width();
\endcode

There are several different settings that affect the table's visual width.
Unless otherwise specified, each table column's width gets computed based
on the contents of each column, and that fixes the table width.

\c minimum_column_widths specifies an explicit column width, in millimeters.
This is a map indexed by column number. The above example sets the width
of columns 0 and 1 to 20 millimeters. This affects only these columns.
Any other columns in the table keep their default minimum width.

\c width optionally sets the table's initial or preferred width.
This is effective only if its more than the minimum table width, whether
defaulted or from \c minimum_column_widths.

Setting \c maximum_table_width specifies the maximum table width. The
resulting table can be wider or narrower than its minimal width.
unlimited_table_width() specifies no maximum table width, the table will be
resizable to any width greater than its minimal size.

\note
The ability of the table to resize itself depends on other elements in
its container. These settings have no effect beyond the resulting table.
Whether, and to what extent, the table ends up resizable depends entirely
on the table's container's layout manager.

\note
\c minimum_column_widths specifies the minimum width of each individual
column. The size of the entire table also includes any borders, and is
included in \c width and \c maximum_width.

\code
new_table.adjustable("table1");

new_table.column_borders={
   {1, "thin_0%"},
   {2, "thin_dashed_0%"},
};
\endcode

\c adjustable() enables interactive adjustments to
the table columns' relative widths by dragging table's borders. This is
independent of the overall column and table width settings. \c adjustable()'s
parameter is the table's unique label.

Note that the table layout manager inherits from the list layout manager
which does not draw borders by default. Set \c column_borders to have
visible borders in the table for best visual results (column border
settings get inherited from
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::new_listlayoutmanager",
which has no borders by default).

\c adjustable()'s parameter is the table's unique identifier. The
adjusted column widths \ref screen_positions "get preserved" and
restored. All tables in the window should have a unique identifier.

\par Accessing and modifying the table's headers

\code
INSERT_LIBX_NAMESPACE::w::element e=tbl->header(0);

INSERT_LIBX_NAMESPACE::w::factory f=tbl->replace_header(0);
\endcode

The initial table headers get specified by the
\ref new_tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager".
An existing table's layout manager's header() returns the header element of
the specified column. replace_header() returns a
\ref factory "factory" for replacing the existing header element with a
new one. Using the returned factory to create and show() a new widget
replaces the existing column's header with a new one.

\par Saving and restoring column widths

\code
INSERT_LIBX_NAMESPACE::new_tablelayoutmanager ntlm;

ntlm.restore("main-table");
\endcode

Assigns a unique label to the table container that's used to restore its
columns' previous sizes. All table widgets in a window must have unique

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
