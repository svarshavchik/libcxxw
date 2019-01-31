/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pane_sliderfwd_h
#define pane_sliderfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN pane_sliderObj;

/*! Slider element.

The implemention object is a focusable element based on a straight_borderObj,
that draws the slider using straight_borderObj.

It is not the owner of the focusable implementation object. This slider
element goes into a focus frame container, which owns the focusable, and
the focus frame container is the actual element in the pane container.

*/

typedef ref<pane_sliderObj> pane_slider;

//! A constant \ref pane_slider "WHATITIS".

//! \see pane_slider

typedef const_ref<pane_sliderObj> const_pane_slider;

//! A nullable pointer reference to a \ref pane_slider "WHATITIS".

//! \see pane_slider

typedef ptr<pane_sliderObj> pane_sliderptr;

//! A nullable pointer reference to a const \ref pane_slider "WHATITIS".

//! \see pane_slider

typedef const_ptr<pane_sliderObj> const_pane_sliderptr;

LIBCXXW_NAMESPACE_END

#endif
