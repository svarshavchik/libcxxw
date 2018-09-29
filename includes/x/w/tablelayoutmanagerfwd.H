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
INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager ntlm{
    []
    (const INSERT_LIBX_NAMESPACE::w::factory &f,
     size_t column)
    {
          f->create_label("Header!")->show();
    }
};

INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager new_table{
   []
   (const x::w::factory &f, size_t i)
   {
         // ...
   }};

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

\code
new_table.width=60;
new_table.maximum_table_width=80;

new_table.unlimited_table_width();

new_table.adjustable_column_widths=true;

\endcode

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
