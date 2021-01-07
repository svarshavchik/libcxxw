/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connectionfwd_h
#define connectionfwd_h

#include <xcb/xcb.h>
#include "x/w/namespace.H"

#include <string>

LIBCXXW_NAMESPACE_START

//! Return a brief identifier for a protocol error.

std::string connection_error(const xcb_generic_error_t *e)
	LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
