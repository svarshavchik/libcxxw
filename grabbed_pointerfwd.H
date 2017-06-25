/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef grabbed_pointerfwd_h
#define grabbed_pointerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

class grabbed_pointerObj;

//! Active pointer grab mcguffin.

//! Returned by grab_pointer() if the pointer grab succeeds.

typedef ref<grabbed_pointerObj> grabbed_pointer;

//! Nullable pointer reference to a grabbed pointer.

typedef ptr<grabbed_pointerObj> grabbed_pointerptr;

LIBCXXW_NAMESPACE_END

#endif
