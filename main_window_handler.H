/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_handler_h
#define main_window_handler_h

#include "generic_window_handler.H"
#include "main_window.H"

LIBCXXW_NAMESPACE_START

//! Common logic for generic input/output windows.

//! Note that this class doesn't handle xcb_create_window and
//! xcb_destroy_window. An implementation class that holds a reference on
//! this handler is responsible for that.

class LIBCXX_HIDDEN main_windowObj::handlerObj :
	public generic_windowObj::handlerObj {

 public:

	using generic_windowObj::handlerObj::handlerObj;

	//! Destructor
	~handlerObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
