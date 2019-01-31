/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_grabbed_pointerfwd_h
#define x_w_impl_grabbed_pointerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN grabbed_pointerObj;

//! Active pointer grab mcguffin.

//! Returned by grab_pointer() if the pointer grab succeeds.

typedef ref<grabbed_pointerObj> grabbed_pointer;

//! Nullable pointer reference to a grabbed pointer.

typedef ptr<grabbed_pointerObj> grabbed_pointerptr;

LIBCXXW_NAMESPACE_END

#endif
