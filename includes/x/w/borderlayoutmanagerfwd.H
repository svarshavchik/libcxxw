/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_borderlayoutmanagerfwd_h
#define x_w_borderlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class borderlayoutmanagerObj;

/*! Draw a border around an element.

A subclass of the \ref singletonlayoutmanager "singleton layout manager"
that draws a border around the display element.

The same effect can be obtained by using the
\ref gridlayoutmanager "grid layout manager", but this is more efficient.

A \ref factory "factory"'s create_bordered_element() creates a container
that uses the border layout manager.

*/

typedef ref<borderlayoutmanagerObj> borderlayoutmanager;

//! A constant \ref borderlayoutmanager "border layout manager".

//! \see borderlayoutmanager

typedef const_ref<borderlayoutmanagerObj> const_borderlayoutmanager;

//! A nullable pointer reference to a \ref borderlayoutmanager "border layout manager".

//! \see borderlayoutmanager

typedef ptr<borderlayoutmanagerObj> borderlayoutmanagerptr;

//! A nullable pointer reference to a const \ref borderlayoutmanager "border layout manager".

//! \see borderlayoutmanager

typedef const_ptr<borderlayoutmanagerObj> const_borderlayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
