/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagelayoutcontainerfwd_h
#define x_w_pagelayoutcontainerfwd_h

#include "x/w/namespace.H"
#include "x/w/container.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN pagelayoutcontainerObj;

/*! Intermediate container used by the \ref pagelayoutmanager "pagelayoutmanager".

A container with a singleton layout manager with the official widget that's
the actual page. Pages are flipped by showing/hiding the intermediate container,
without affecting the apparent visibility of each page element.

*/

typedef ref<pagelayoutcontainerObj> pagelayoutcontainer;

//! A constant \ref pagelayoutcontainer "page container".

//! \see pagelayoutcontainer

typedef const_ref<pagelayoutcontainerObj> const_pagelayoutcontainer;

//! A nullable pointer reference to a \ref pagelayoutcontainer "page container".

//! \see pagelayoutcontainer

typedef ptr<pagelayoutcontainerObj> pagelayoutcontainerptr;

//! A nullable pointer reference to a const \ref pagelayoutcontainer "page container".

//! \see pagelayoutcontainer

typedef const_ptr<pagelayoutcontainerObj> const_pagelayoutcontainerptr;

LIBCXXW_NAMESPACE_END

#endif
