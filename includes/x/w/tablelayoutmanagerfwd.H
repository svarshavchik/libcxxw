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


\code
INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager new_table{
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &f,
     size_t column)
    {
          f->create_label("Header!")->show();
    }
};

new_table.columns=2;

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
new_table.width=60;
new_table.maximum_table_width=80;

new_table.unlimited_table_width();
\endcode

The table's width gets automatically computed based on how wide the table's
elements are. \c width sets a the initial table width, specified in
millimeters. This is effective only if its more than the default, minimal
table width.

Setting \c maximum_table_width specifies the maximum table width. The
resulting table can be wider or narrower than its minimal width.
unlimited_table_width() specifies no maximum table width, the table will be
resizable to any width greater than its minimal size.

\note
The ability of the table to resize itself depends on other elements in
its container. These settings have no effect beyond the resulting table.
Whether, and to what extent, the table ends up resizable depends entirely
on the table's container's layout manager.

\code
new_table.adjustable_column_widths=true;

new_table.column_borders={
   {1, "thin_0%"},
   {2, "thin_dashed_0%"},
};
\endcode

Setting \c adjustable_column_widths enables interactive adjustments to
the table columns' relative widths by dragging table's borders. This is
independent of the overall table width settings.

Note that the table layout manager inherits from the list layout manager
which does not draw borders by default. Set \c column_borders to have
visible borders in the table, for best visual results (column border
settings get inherited from
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::new_listlayoutmanager",
which has no borders by default).

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
new one. Using the returned factory to create and show() a new display element
replaces the existing column's header with a new one.

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
