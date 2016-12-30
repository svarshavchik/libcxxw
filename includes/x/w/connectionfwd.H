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
//!
//! \par Screen info
//!
//! \code
//! size_t n=conn->screens();
//!
//! size_t n=conn->default_screen();
//! \endcode
//!
//! \par Connection mcguffin
//!
//! \code
//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=conn->mcguffin();
//! \endcode
//!
//! \c mcguffin() returns an opaque object representing the underlying
//! connection to the display server. Even after all public objects related to
//! a connection go out of scope and get destroyed, a background thread will
//! briefly run, just long enough to wind up its business, before the
//! connection to the display server is truly closed.
//!
//! \c mcguffin() returns an opaque mcguffin. When the mcguffin gets destroyed
//! the underlying display server connection is truly gone, and all resources
//! have been released.

typedef ref<connectionObj> connection;

//! A constant connection to the display server

//! \see connection

typedef const_ref<connectionObj> const_connection;

LIBCXXW_NAMESPACE_END

#endif
