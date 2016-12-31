/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "window_handler.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::window_handlerObj);

LIBCXXW_NAMESPACE_START

window_handlerObj
::window_handlerObj(const ref<connectionObj::implObj::threadObj> &thread_)
	: xid_t<xcb_window_t>(thread_)
{
}

window_handlerObj::~window_handlerObj() noexcept=default;

LIBCXXW_NAMESPACE_END
