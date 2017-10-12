/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themedimfwd_h
#define themedimfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class themedimObj;

//! A dimension specified by the default theme.

//! The constructor takes the name of a dimension. pixels(IN_THREAD) returns
//! the number of pixels in the dimension.
//!
//! Both the constructor, and theme_updated() take an additional parameter
//! referencing the screen whose theme gets checked.

typedef ref<themedimObj> themedim;

//! A constant \ref themedim "theme specified dimension".

typedef const_ref<themedimObj> const_themedim;

LIBCXXW_NAMESPACE_END

#endif
