/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_peephole_containerfwd_h
#define x_w_pane_peephole_containerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class pane_peephole_containerObj;

/*! Pane peephole element.

The actual element in a container with a
\ref panelayoutmanager "pane layout manager" is a container containing
a \ref pane_peephole "pane peephole" with its scrollbars.

This is a subclass of a focusable container. The scrollbars are focusable,
and this container combines both scrollbars into a single focusable
pseudo-element, so that the scrollbars' tabbing order gets handled as a
single group.

*/

typedef ref<pane_peephole_containerObj> pane_peephole_container;

//! A constant \ref pane_peephole_container "pane peephole container".

//! \see pane_peephole_container

typedef const_ref<pane_peephole_containerObj> const_pane_peephole_container;

//! A nullable pointer reference to a \ref pane_peephole_container "pane peephole container".

//! \see pane_peephole_container

typedef ptr<pane_peephole_containerObj> pane_peephole_containerptr;

//! A nullable pointer reference to a const \ref pane_peephole_container "pane peephole container".

//! \see pane_peephole_container

typedef const_ptr<pane_peephole_containerObj> const_pane_peephole_containerptr;

LIBCXXW_NAMESPACE_END

#endif
