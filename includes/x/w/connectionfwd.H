#ifndef x_w_connectionfwd_h
#define x_w_connectionfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class connectionObj;

//! A connection to the display server

//! \code
//! auto conn=INSERT_LIBX_NAMESPACE::w::connection::create();
//!
//! auto conn=INSERT_LIBX_NAMESPACE::w::connection::create("localhost:1.0");
//! \endcode
//!
//! Creates an object representing a connection to the display SERVER.

typedef ref<connectionObj> connection;

//! A constant connection to the display server

//! \see connection

typedef const_ref<connectionObj> const_connection;

LIBCXXW_NAMESPACE_END

#endif
