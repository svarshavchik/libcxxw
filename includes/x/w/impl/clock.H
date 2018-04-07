/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_clock_h
#define x_w_impl_clock_h

#include <x/w/namespace.H>
#include <chrono>

LIBCXXW_NAMESPACE_START

//! A clock for regularly-scheduled callbacks.

typedef std::chrono::steady_clock tick_clock_t;

LIBCXXW_NAMESPACE_END

#endif
