/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connection_threadfwd_h
#define x_w_connection_threadfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN connection_threadObj;
class LIBCXX_HIDDEN containers_2_recalculate_map;
class LIBCXX_HIDDEN containers_2_batch_recalculate_set;
class LIBCXX_HIDDEN elements_2_batch_showhide_map;


//! Internal library execution thread.

typedef ref<connection_threadObj> connection_thread;

//! Some class methods can only be executed by the internal connection thread

//! Syntactic sugar for designating class methods and library
//! functions that are callable only by the library's internal connection
//! thread.
//!
//! \code
//! void foo(ONLY IN_THREAD, int);
//!
//!
//! void bar(ONLY IN_THREAD)
//! {
//!     // ...
//!
//!     foo(IN_THREAD);
//! }
//! \endcode
//!
//! The library functions' first parameter gets declared as "ONLY IN_THREAD",
//! and the function call other thread-only functions by specifying
//! "IN_THREAD" as their first parameter.
//!
//! This provides for a lightweight compile-time enforcement of thread-only
//! methods and functions that are callable only by other thread-executed
//! code.

typedef const connection_thread &ONLY;

//! Alternative syntactic sugar for \ref ONLY "thread-only invoked callbacks"

//! \code
//! element->on_state_update([]
//!                          (THREAD_CALLBACK, ...)
//!                          {
//!                                method(IN_THREAD);
//!                          });
//! \endcode
//!
//! Alternative syntactic sugar for declaring callbacks that get invoked
//! by the library's internal execution thread.

typedef ONLY THREAD_CALLBACK;

LIBCXXW_NAMESPACE_END

using LIBCXX_NAMESPACE::w::ONLY;
using LIBCXX_NAMESPACE::w::THREAD_CALLBACK;

#endif
