/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_singletonlayoutmanagerfwd_h
#define x_w_singletonlayoutmanagerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC singletonlayoutmanagerObj;

/*! A layout manager for one element only.

Shared code for layout managers that have a single child element.

\code
INSERT_LIBX_NAMESPACE::element e=l->get();

INSERT_LIBX_NAMESPACE::factory f=l->replace();
\endcode

get() returns the current element. replace() returns an
\ref factory "widget factory". Using the factory to create and show()
a new widget replaces the previous widget in the container.

*/

typedef ref<singletonlayoutmanagerObj> singletonlayoutmanager;

//! A constant \ref singletonlayoutmanager "singleton layout manager".

//! \see singletonlayoutmanager

typedef const_ref<singletonlayoutmanagerObj> const_singletonlayoutmanager;

//! A nullable pointer reference to a \ref singletonlayoutmanager "singleton layout manager".

//! \see singletonlayoutmanager

typedef ptr<singletonlayoutmanagerObj> singletonlayoutmanagerptr;

//! A nullable pointer reference to a const \ref singletonlayoutmanager "singleton layout manager".

//! \see singletonlayoutmanager

typedef const_ptr<singletonlayoutmanagerObj> const_singletonlayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
