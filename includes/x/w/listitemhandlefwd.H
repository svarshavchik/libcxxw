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
without holding a strong reference on the display element.

The handle gives a way of stashing away something that can be used to
conveniently update a particular item in the list, without going through
the motions of memoizing its list item number, and going through the motions
of instantiating the layout manager and updating the list item. This object
does not hold any strong references on the display element. If the underlying
list item gets removed from the list, or the list itself gets removed, this
object automatically becomes an effective no-op, and attempts to update the
now non-existent list item do nothing.

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
