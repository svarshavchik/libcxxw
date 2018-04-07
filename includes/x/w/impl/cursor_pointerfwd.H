/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_cursor_pointerfwd_h
#define x_w_impl_cursor_pointerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class cursor_pointerObj;

/*! A cursor pointer.

A cursor pointer created from an icon.

Do not create this object directly. Use icon's create_cursor(), which
implements a cache.

Implements initialize() and theme_updated(). initialize() should be called
before using the icon in the connection thread for the first time. And the
connection thread calls theme_updated() when the display theme changes.

Both initialize() and theme_updated() return the same icon object if the
theme hasn't changed, or a new icon object.

*/

typedef ref<cursor_pointerObj> cursor_pointer;

//! A constant \ref cursor_pointer "cursor_pointer".

//! \see cursor_pointer

typedef const_ref<cursor_pointerObj> const_cursor_pointer;

//! A nullable cursor_pointer reference to a \ref cursor_pointer "cursor_pointer".

//! \see cursor_pointer

typedef ptr<cursor_pointerObj> cursor_pointerptr;

//! A nullable cursor_pointer reference to a const \ref cursor_pointer "cursor_pointer".

//! \see cursor_pointer

typedef const_ptr<cursor_pointerObj> const_cursor_pointerptr;

LIBCXXW_NAMESPACE_END

#endif
