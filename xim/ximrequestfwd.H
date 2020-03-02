/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximrequestfwd_H
#define ximrequestfwd_H

#include <x/ref.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN ximrequestObj;

//! A synchronous request pending with the XIM server.

//! These requests get placed in \ref ximxserver "ximserver"'s request_queue.
//!
//! The request at the head of the queue is sent to the XIM server.
//!
//! When a response is received from the XIM server, the queue is popped,
//! and the response results in the appropriate method of this object getting
//! invoked.
//!
//! The default methods in this class report an error. The actual request
//! is a subclass of this that implements the request-specific response method.
//!
//! The expected result is that the server will send a response that's
//! appropriate for the request.

typedef ref<ximrequestObj> ximrequest;

//! A nullable pointer reference to a \ref ximrequest "synchronous XIM request".

typedef ptr<ximrequestObj> ximrequestptr;

LIBCXXW_NAMESPACE_END
#endif
