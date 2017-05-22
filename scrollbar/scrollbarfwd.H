/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbarfwd_h
#define scrollbarfwd_h

#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class scrollbarObj;

struct scrollbar_orientation;

extern const scrollbar_orientation horizontal_scrollbar LIBCXX_HIDDEN;

extern const scrollbar_orientation vertical_scrollbar LIBCXX_HIDDEN;

//! A scrollbar element.

typedef ref<scrollbarObj> scrollbar;

//! A nullable pointer reference to an scrollbar.

//! \see scrollbar

typedef ptr<scrollbarObj> scrollbarptr;

//! A reference to a constant scrollbar object.

//! \see scrollbar

typedef const_ref<scrollbarObj> const_scrollbar;

//! A nullable pointer reference to a constant scrollbar object.

//! \see scrollbar

typedef const_ptr<scrollbarObj> const_scrollbarptr;

LIBCXXW_NAMESPACE_END

#endif
