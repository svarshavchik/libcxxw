/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_peepholefwd_h
#define x_w_pane_peepholefwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class pane_peepholeObj;

/*! A peephole for the contents of a pane.

This is a child element of the
\ref pane_peephole_container "pane peephole container" that uses the
peephole layout manager to scroll the "real" element in the pane.

*/

typedef ref<pane_peepholeObj> pane_peephole;

//! A constant \ref pane_peephole "pane contents peephole".

//! \see pane_peephole

typedef const_ref<pane_peepholeObj> const_pane_peephole;

//! A nullable pointer reference to a \ref pane_peephole "pane contents peephole".

//! \see pane_peephole

typedef ptr<pane_peepholeObj> pane_peepholeptr;

//! A nullable pointer reference to a const \ref pane_peephole "pane contents peephole".

//! \see pane_peephole

typedef const_ptr<pane_peepholeObj> const_pane_peepholeptr;

LIBCXXW_NAMESPACE_END

#endif
