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
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create();
//!
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create(1);
//!
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create(INSERT_LIBX_NAMESPACE::w::connection::create("localhost:0.0"));
//!
//! auto s=INSERT_LIBX_NAMESPACE::w::screen::create(INSERT_LIBX_NAMESPACE::w::connection::create("localhost:0.0"), 1);
//!
//! INSERT_LIBX_NAMESPACE::w::connection conn=s->get_connection()
//!
//! \endcode
//!
//! create() returns a screen object for the default display of the default
//! display server connection, a specific screen object, or a screen object
//! for the explicitly specified connection, depending on the parameters.
//!
//! get_connection() returns the existing screen object's
//! \ref connection "connection object".

typedef ref<screenObj, screenBase> screen;

//! A constant screen

typedef const_ref<screenObj, screenBase> const_screen;

LIBCXXW_NAMESPACE_END

#endif
