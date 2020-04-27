/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_containerfwd_h
#define x_w_containerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC containerObj;

//! Methods common to all display containers.

//! A container contains other widgets, and a
//! \ref layoutmanager "layout manager" object
//! that manages them.
//!
//! Creating a widget that's a container requires that
//! a \ref new_layoutmanager "new layout manager" gets created first, then
//! used to create the container.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::layoutmanager manager=container->get_layoutmanager();
//! \endcode
//!
//! get_layoutmanager() returns the container's
//! \ref layoutmanager "layout manager".
//!
//! \code
//! auto manager=container->gridlayout();
//! \endcode
//!
//! gridlayout(), tablelayout(), and other aliases are shortcuts for
//! calling get_layoutmanager() and converting it to the corresponding
//! type, such as \ref gridlayoutmanager "INSERT_LIBX_NAMESPACE::w::gridlayoutmanager"
//! and \ref tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::tablelayoutmanager".
//!
//! And exception gets thrown if the container uses a different layout manager.

typedef ref<containerObj> container;

//! A nullable pointer reference to a \ref container "container".

typedef ptr<containerObj> containerptr;

//! A constant container.

//! \see container

typedef const_ref<containerObj> const_container;

//! A nullable pointer reference to a constant \ref container "container".

typedef const_ptr<containerObj> const_containerptr;

LIBCXXW_NAMESPACE_END

#endif
