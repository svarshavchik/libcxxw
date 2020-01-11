/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_get_window_handler_h
#define x_w_impl_get_window_handler_h

#include <x/w/generic_windowobj.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC get_window_handlerObj;

//! Common superclass for objects that implement get_window_handler()

class get_window_handlerObj {

	generic_windowObj::handlerObj &generic_window_handlerObj;
public:
	inline get_window_handlerObj(generic_windowObj::handlerObj &h)
		: generic_window_handlerObj{h}
	{
	}

	//! Return my window

	inline generic_windowObj::handlerObj &get_window_handler()
	{
		return generic_window_handlerObj;
	}

	//! Return my window
	const generic_windowObj::handlerObj &get_window_handler() const
	{
		return generic_window_handlerObj;
	}
};

LIBCXXW_NAMESPACE_END

#endif
