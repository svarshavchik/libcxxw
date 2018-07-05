/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_get_window_handler_h
#define x_w_impl_get_window_handler_h

#include <x/w/generic_windowobj.H>

LIBCXXW_NAMESPACE_START

//! Common superclass for objects that implement get_window_handler()

class get_window_handlerObj {

public:
	//! Return my window

	virtual generic_windowObj::handlerObj &get_window_handler()=0;

	//! Return my window
	virtual const generic_windowObj::handlerObj &get_window_handler() const=0;
};

LIBCXXW_NAMESPACE_END

#endif
