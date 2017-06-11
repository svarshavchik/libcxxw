/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_containerfwd_h
#define x_w_focusable_containerfwd_h

#include <x/w/containerfwd.H>
#include <x/w/focusablefwd.H>

LIBCXXW_NAMESPACE_START

class focusable_containerObj;

//! A focusable display element that's implemented as an internal container.

//! Base class for display elements that multiply-inherit from a
//! \ref container "container" and a \ref focusable "focusable".
//!
//! These display elements are treated as individual display elements,
//! but provide a layout manager-based means to modify their contents.

typedef ref<focusable_containerObj> focusable_container;

//! A constant focusable container.

//! \see focusable_container

typedef const_ref<focusable_containerObj> const_focusable_container;

//! A nullable pointer reference to a focusable container.

//! \see focusable_container

typedef ptr<focusable_containerObj> focusable_containerptr;

//! A nullable pointer reference to a constant focusable container.

//! \see focusable_container

typedef const_ptr<focusable_containerObj> const_focusable_containerptr;

LIBCXXW_NAMESPACE_END

#endif
