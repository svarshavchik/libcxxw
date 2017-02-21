/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_containerfwd_h
#define x_w_containerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class containerObj;

//! Methods common to all display containers.

//! A container contains other display elements, and a
//! \ref layoutmanager "layout manager" object
//! that manages them.
//!
//! Creating a display element that's a container requires that
//! a \ref new_layoutmanager "new layout manager" gets created first, then
//! used to create the container.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::layoutmanager manager=container->get_layoutmanager();
//! \endcode
//!
//! get_layoutmanager() returns the container's
//! \ref layoutmanager "layout manager".

typedef ref<containerObj> container;

//! A constant generic window handler

//! \see container

typedef const_ref<containerObj> const_container;

LIBCXXW_NAMESPACE_END

#endif
