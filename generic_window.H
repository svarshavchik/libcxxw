/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_h
#define generic_window_h

#include "x/w/generic_window.H"
#include "x/w/types.H"
#include "x/w/rectangle.H"

#include "element.H"
#include "connection_threadfwd.H"
#include <xcb/xcb.h>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Generic window internal implemenation object.

//! The constructor calls xcb_create_window(), and installs the
//! handler in the connection thread.
//!
//! The destructor removes the handler from the connection thread, and
//! installs the handler's xid in the connection thread's destroyed_xids,
//! then installs a destructor callback on the handler object. When all
//! references to the handler object go out of scope and it gets destroyed,
//! xcb_destroy_window().

class LIBCXX_HIDDEN generic_windowObj::implObj : virtual public obj {

 public:
	//! Constructor

	//! The constructor installs the window_handler in the connection
	//! thread's window_handler map.

	implObj(const ref<handlerObj> &handler);

	//! Destructor

	//! The destructor removes the window_handler from the connection
	//! thread's window_handler map.
	~implObj();

	const ref<handlerObj> handler;

	//! Return my connection thread
	connection_thread thread();

};

LIBCXXW_NAMESPACE_END

#endif
