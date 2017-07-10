/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef run_as_h
#define run_as_h

#include "xid_t.H"
#include "connection_thread.H"
#include "element.H"
#include "generic_window_handler.H"
#include "screen.H"

LIBCXXW_NAMESPACE_START

// A child of elementObj::implObj can use THREAD to find the connection
// thread.

#define THREAD get_window_handler().thread()

LIBCXXW_NAMESPACE_END

#endif
