/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef straight_borderfwd_h
#define straight_borderfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"
#include "x/w/container.H"
#include "x/w/grid_elementfwd.H"
#include "current_border_implfwd.H"

LIBCXXW_NAMESPACE_START

class straight_borderObj;
class straight_borderBase;

//! A straight border element in a grid

//! This is an internal element object that's automatically created by
//! \ref gridlayoutmanager "grid layout manager" for drawing horizontal
//! and vertical grid borders.

typedef ref<straight_borderObj, straight_borderBase> straight_border;

//! A nullable pointer reference to a \ref straight_border "straight border".

typedef ptr<straight_borderObj, straight_borderBase> straight_borderptr;

//! A reference to a constant \ref straight_border "straight border".

typedef const_ref<straight_borderObj, straight_borderBase> const_straight_border;

//! A nullable pointer reference to a constant \ref straight_border "straight border".

typedef const_ptr<straight_borderObj, straight_borderBase> const_straight_borderptr;


//! A signature for a border factory.

//! create_horizontal_border() and create_vertical_border() take
//! the following list of parameters:
//!
//! - the parent container (the grid container)
//! - two elements, on either side of the border (each one possibly
//!   null, for an element on the edge of the border).
//! - the default border for the grid.

typedef straight_border
straight_border_factory_t(const ref<containerObj::implObj> &c,
			  const grid_elementptr &e1,
			  const grid_elementptr &e2,
			  const current_border_implptr &d);

//! A signature for a border updater.

//! Parameters for update_horizontal_border() and
//! update_vertical_border().

typedef straight_border
straight_border_update_t(IN_THREAD_ONLY,
			 const straight_border &b,
			 const grid_elementptr &e1,
			 const grid_elementptr &e2,
			 const current_border_implptr &d);

LIBCXXW_NAMESPACE_END

#endif