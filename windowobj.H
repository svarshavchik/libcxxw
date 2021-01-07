/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef windowobj_h
#define windowobj_h

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/logger.H>

#include "window_handlerfwd.H"
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

//! Owner of a \ref window_handler "window handler"

//! The constructor installs the handler in the connection thread.
//!
//! The destructor removes the handler from the connection thread, and
//! installs the handler's xid in the connection thread's destroyed_xids,
//! then installs a destructor callback on the handler object. When all
//! references to the handler object go out of scope and it gets destroyed.

class LIBCXX_HIDDEN windowObj : virtual public obj {

	LOG_CLASS_SCOPE;

public:

	//! The window handler

	const ref<window_handlerObj> handler;

	//! Constructor

	//! Registered the handler with the connection thread.

	windowObj(const ref<window_handlerObj> &handler);

	//! Deregisteres the handler from the connection thread.

	~windowObj();
};

LIBCXXW_NAMESPACE_END

#endif
