/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholed_item_containerfwd_h
#define x_w_peepholed_item_containerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN peepholed_item_containerObj;

/*! The actual container that uses the item layout manager.

The actual container with the item layout manager. This is actually the
container in a peephole. The externally-visible container is a container
with a peephole, that's rigged to return this container's layoutmanager,
when asked to produce the layout manager.

//! Uses the always_visible_elementObj mixin because the public item
//! layout manager container uses the nonrecursive_visibility mixin. The
//! peepholed container is always visible.
//!
//! Uses the theme_font_elementObj mixin. This is a reference font to provide
//! the nominal increments for the peephole scrollbars.

*/

typedef ref<peepholed_item_containerObj> peepholed_item_container;

//! A constant \ref peepholed_item_container "container with the item layout manager".

//! \see peepholed_item_container

typedef const_ref<peepholed_item_containerObj> const_peepholed_item_container;

//! A nullable pointer reference to a \ref peepholed_item_container "container with the item layout manager".

//! \see peepholed_item_container

typedef ptr<peepholed_item_containerObj> peepholed_item_containerptr;

//! A nullable pointer reference to a const \ref peepholed_item_container "container with the item layout manager".

//! \see peepholed_item_container

typedef const_ptr<peepholed_item_containerObj> const_peepholed_item_containerptr;

LIBCXXW_NAMESPACE_END

#endif
