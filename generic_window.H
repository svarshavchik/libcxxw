/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef generic_window_h
#define generic_window_h

#include "x/w/generic_window.H"
#include "x/w/types.H"
#include "x/w/rectangle.H"

#include "x/w/connection_threadfwd.H"
#include "windowobj.H"

LIBCXXW_NAMESPACE_START

//! Generic window internal implemenation object.

class LIBCXX_HIDDEN generic_windowObj::implObj : public windowObj {

 public:
	//! Constructor

	//! The constructor installs the window_handler in the connection
	//! thread's window_handler map.

	implObj(const ref<handlerObj> &handler);

	//! Destructor

	//! The destructor removes the window_handler from the connection
	//! thread's window_handler map.
	~implObj();

	//! The window handler
	const ref<handlerObj> handler;

	//! Return my connection thread
	connection_thread thread();

};

LIBCXXW_NAMESPACE_END

#endif
