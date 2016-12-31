/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screenfwd_h
#define x_w_screenfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class screenObj;
class screenBase;

//! A screen on a display server.
//!
//! \code
//!
//! // Open the default screen on the default display connection
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create();
//!
//! // Open the specified screen on the default display connection
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create(1);
//!
//! // Open the default screen on the given connection
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create(INSERT_LIBX_NAMESPACE::w::connection::create("localhost:0.0"));
//!
//! // Open the specified screen on the given connection
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create(INSERT_LIBX_NAMESPACE::w::connection::create("localhost:0.0"), 1);
//! \endcode

typedef ref<screenObj, screenBase> screen;

//! A constant screen

typedef const_ref<screenObj, screenBase> const_screen;

LIBCXXW_NAMESPACE_END

#endif
