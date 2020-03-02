/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_themedimfwd_h
#define x_w_impl_themedimfwd_h

#include <x/ptrfwd.H>
#include <x/w/metrics/axisfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC themedimObj;
struct themedim_element_init;

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
