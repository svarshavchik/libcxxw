/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_elementfwd_h
#define x_w_list_elementfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class list_elementObj;

/*! The singleton element managed by the \ref listlayoutmanager "list layout manager".

The list layout manager uses the layout manager framework to manage list items.

List items are not discrete display elements; but within the framework
the list layout manager is set up as containing this display element.

The \ref list_element "list element" internally manages individual cell
objects, that represent the items in the list, with the list layout manager
using them to implement the list concept.

*/

typedef ref<list_elementObj> list_element;

//! A constant \ref list_element "text list element".

//! \see list_element

typedef const_ref<list_elementObj> const_list_element;

//! A nullable pointer reference to a \ref list_element "text list element".

//! \see list_element

typedef ptr<list_elementObj> list_elementptr;

//! A nullable pointer reference to a const \ref list_element "text list element".

//! \see list_element

typedef const_ptr<list_elementObj> const_list_elementptr;

LIBCXXW_NAMESPACE_END

#endif
