/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_firstlistitemcontainerfwd_h
#define x_w_firstlistitemcontainerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class firstlistitemcontainerObj;

/*! First element in a list layout manager's item.

Each item in a list layoutmanager is a row, with one or more elements.

The first element in the row carries some additional item-specific
information.

*/

typedef ref<firstlistitemcontainerObj> firstlistitemcontainer;

//! A constant \ref firstlistitemcontainer "first list item element".

//! \see firstlistitemcontainer

typedef const_ref<firstlistitemcontainerObj> const_firstlistitemcontainer;

//! A nullable pointer reference to a \ref firstlistitemcontainer "first list item element".

//! \see firstlistitemcontainer

typedef ptr<firstlistitemcontainerObj> firstlistitemcontainerptr;

//! A nullable pointer reference to a const \ref firstlistitemcontainer "first list item element".

//! \see firstlistitemcontainer

typedef const_ptr<firstlistitemcontainerObj> const_firstlistitemcontainerptr;

LIBCXXW_NAMESPACE_END

#endif
