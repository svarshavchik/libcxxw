/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef activated_in_threadfwd_h
#define activated_in_threadfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class activated_in_threadObj;

/*! An object with activated() and enabled() invoked by the connection thread.


*/

typedef ref<activated_in_threadObj> activated_in_thread;

//! A constant \ref activated_in_thread "activatable object".

//! \see activated_in_thread

typedef const_ref<activated_in_threadObj> const_activated_in_thread;

//! A nullable pointer reference to a \ref activated_in_thread "activatable object".

//! \see activated_in_thread

typedef ptr<activated_in_threadObj> activated_in_threadptr;

//! A nullable pointer reference to a const \ref activated_in_thread "activatable object".

//! \see activated_in_thread

typedef const_ptr<activated_in_threadObj> const_activated_in_threadptr;

LIBCXXW_NAMESPACE_END

#endif
