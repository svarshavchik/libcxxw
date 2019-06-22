/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listitemhandlefwd_h
#define x_w_listitemhandlefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN listitemhandleObj;

/*! A reference handle for an item in a \ref listlayoutmanager "list".

Provides the means for referring to an item in a list, combo-box, or a
popup menu (whose layout managers derives from the list layout manager)
without holding a strong reference on the widget.

List item handlers are returned when
\ref get_new_items "INSERT_LIBX_NAMESPACE::w::get_new_items" is requested
when calling of the list layout manager methods that create new list items.

The handle gives a way of having weak references to items in the list, and
make it possible to
conveniently update a particular item in the list, without going through
the motions of keeping track of its list item number, and going through
the motions of formally constructing the layout manager object
and updating the list item. List item handles support the
following operation on their list item:

\code
bool enabled=handle->enabled();

handle->enabled(true);

handle->enabled(IN_THREAD, true);
\endcode

enabled() enables or disables the handle's item, or indicates whether
the item is currently enabled.


\code
handle->on_status_update([]
                         (ONLY IN_THREAD,
                          const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                         {
                         });

handle->on_status_update(IN_THREAD,
                         []
                         (ONLY IN_THREAD,
                          const INSERT_LIBX_NAMESPACE::w::list_item_status_info_t &)
                         {
                         });
\endcode

on_status_update() installs or replaces the list item's status callback.

\code
INSERT_LIBX_NAMESPACE::w::listlayoutmanagerptr l=handle->get_item_layoutmanager();
\endcode

Return the sub-menu's layout manager, if this is a submenu entry in a menu,
and the menu has not been destroyed.

This handle does not hold any strong references on the widget. If
the underlying
list item gets removed from the list, or the list itself gets removed, this
object automatically becomes an effective no-op, and attempts to update the
now non-existent list item do nothing. enabled() always returns false.
get_item_layoutmanager() returns a null ptr.

The new callback installed by on_status_update() gets immediately invoked
with the trigger value of \ref initial "INSERT_LIBX_NAMESPACE::w::initial",
reporting the list item's current selection status. The callback does not
get invoked if the list item does not exist any more.

*/

typedef ref<listitemhandleObj> listitemhandle;

//! A constant \ref listitemhandle "WHATITIS".

//! \see listitemhandle

typedef const_ref<listitemhandleObj> const_listitemhandle;

//! A nullable pointer reference to a \ref listitemhandle "WHATITIS".

//! \see listitemhandle

typedef ptr<listitemhandleObj> listitemhandleptr;

//! A nullable pointer reference to a const \ref listitemhandle "WHATITIS".

//! \see listitemhandle

typedef const_ptr<listitemhandleObj> const_listitemhandleptr;

LIBCXXW_NAMESPACE_END

#endif
