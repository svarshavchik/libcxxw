/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_synchronized_axisfwd_h
#define x_w_synchronized_axisfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN synchronized_axisObj;

/*! Synchronized axis.

An internal structure used to link the widths of columns from two or more
\ref listlayoutmanager "selection lists".

Creating a new selection list using the same synchronized_axis will result
in the selection lists' columns to have the same width provided that the
selection lists themselves are sized the same width by their respective
containers.

*/

typedef ref<synchronized_axisObj> synchronized_axis;

//! A constant \ref synchronized_axis "synchronized axis".

//! \see synchronized_axis

typedef const_ref<synchronized_axisObj> const_synchronized_axis;

//! A nullable pointer reference to a \ref synchronized_axis "synchronized axis".

//! \see synchronized_axis

typedef ptr<synchronized_axisObj> synchronized_axisptr;

//! A nullable pointer reference to a const \ref synchronized_axis "synchronized axis".

//! \see synchronized_axis

typedef const_ptr<synchronized_axisObj> const_synchronized_axisptr;

LIBCXXW_NAMESPACE_END

#endif
