/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_grid_elementfwd_h
#define x_w_impl_grid_elementfwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN grid_elementObj;

//! An widget in a grid.

//! An object that represents an widget in a grid, and its metadata;
//! namely its "width" and "height" (number of grid cells it spans),
//! and its custom borders, if any.
//!
//! \internal

typedef ref<grid_elementObj> grid_element;

//! A nullable pointer reference to a \ref grid_element.

typedef ptr<grid_elementObj> grid_elementptr;

//! A reference to a constant \ref grid_element.

typedef ref<grid_elementObj> grid_element;

//! A nullable pointer reference to a constant \ref grid_element.

typedef ptr<grid_elementObj> grid_elementptr;

LIBCXXW_NAMESPACE_END

#endif
