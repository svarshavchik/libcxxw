/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "generic_window.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "connection_info.H"
#include "impl_connection_thread.H"
#include "screen.H"
#include "connection.H"

LIBCXXW_NAMESPACE_START

generic_windowObj::implObj::implObj(const ref<handlerObj> &handler)
	: windowObj(handler), handler(handler)
{
}

generic_windowObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
