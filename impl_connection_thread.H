/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef generic_window_impl_h
#define generic_window_impl_h

#include "generic_window.H"
#include "generic_window_handler.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

inline connection_thread generic_windowObj::implObj::thread()
{
	return handler->thread();
}

LIBCXXW_NAMESPACE_END

#endif
