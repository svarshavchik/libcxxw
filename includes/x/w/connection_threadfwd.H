/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connection_threadfwd_h
#define x_w_connection_threadfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class connection_threadObj;

//! Internal library execution thread.

typedef ref<connection_threadObj> connection_thread;

LIBCXXW_NAMESPACE_END

#endif
