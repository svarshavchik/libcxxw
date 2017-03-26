/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusablefwd_h
#define x_w_focusablefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class focusableObj;
class focusableImplObj;

//! Methods common to all display element that can receive input focus.

typedef ref<focusableObj> focusable;

//! A nullable pointer reference to a focusable display element.

typedef ptr<focusableObj> focusableptr;

//! A reference to a constant focusable display element.

//! \see focusable

typedef const_ref<focusableObj> const_focusable;

//! A nullable pointer reference to a constant focusable display element.

//! \see focusable

typedef const_ptr<focusableObj> const_focusableptr;

LIBCXXW_NAMESPACE_END

#endif
