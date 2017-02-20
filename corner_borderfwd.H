/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef corner_borderfwd_h
#define corner_borderfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class corner_borderObj;

//! A corner border element in a grid

//! This is an internal element object that's automatically created by
//! \ref gridlayoutmanager "grid layout manager" for drawing corner borders.
//! and vertical grid borders.

typedef ref<corner_borderObj> corner_border;

//! A nullable pointer reference to a \ref corner_border "corner border".

typedef ptr<corner_borderObj> corner_borderptr;

//! A reference to a constant \ref corner_border "corner border".

typedef const_ref<corner_borderObj> const_corner_border;

//! A nullable pointer reference to a constant \ref corner_border "corner border".

typedef const_ptr<corner_borderObj> const_corner_borderptr;

LIBCXXW_NAMESPACE_END

#endif
