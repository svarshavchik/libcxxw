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

The handle gives a way of having weak reference to items in the list to
make it possible to
conveniently update a particular item in the list, without going through
the motions of memorizing its list item number, and going through the motions
of instantiating the layout manager and updating the list item. Only a limited
number of operations are currently possible:

\code
bool enabled=handle->enabled();

handle->enabled(true);

handle->enabled(IN_THREAD, true);
\endcode

enabled() enables or disables the handle's item, or indicates whether
the item is currently enabled.

This handle does not hold any strong references on the widget. If
the underlying
list item gets removed from the list, or the list itself gets removed, this
object automatically becomes an effective no-op, and attempts to update the
now non-existent list item do nothing. enabled() always returns false.

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
