/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_child_elementfwd_h
#define x_w_impl_child_elementfwd_h

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC child_elementObj;

//! Tag for the background color

struct child_element_bgcolor;

typedef ref<child_elementObj> child_element;

struct LIBCXX_HIDDEN child_element_init_params;

LIBCXXW_NAMESPACE_END

#endif
