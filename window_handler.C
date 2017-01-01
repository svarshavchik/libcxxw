/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "window_handler.H"
#include "connection_thread.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::window_handlerObj);

LIBCXXW_NAMESPACE_START

window_handlerObj
::window_handlerObj(IN_THREAD_ONLY): xid_t<xcb_window_t>(thread_)
{
}

window_handlerObj::~window_handlerObj() noexcept=default;


void window_handlerObj::change_property(IN_THREAD_ONLY,
					uint8_t mode,
					xcb_atom_t property,
					xcb_atom_t type,
					uint8_t format,
					uint32_t data_len,
					void *data)
{
	xcb_change_property(IN_THREAD->info->conn, mode, id(), property,
			    type, format, data_len, data);
}

void window_handlerObj::client_message_event(IN_THREAD_ONLY,
					     const xcb_client_message_event_t *)
{
}

LIBCXXW_NAMESPACE_END
