/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "sync_counter.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

sync_counter::sync_counter(ONLY IN_THREAD)
	: new_xid{IN_THREAD}
{
	xcb_sync_create_counter(conn()->conn, id(), {0, 0});
}

sync_counter::~sync_counter()
{
	xcb_sync_destroy_counter(conn()->conn, id());
}

void sync_counter::set(ONLY IN_THREAD, int64_t new_value)
{
	xcb_sync_set_counter(conn()->conn, id(),
			     {static_cast<int32_t>(new_value >> 32),
			      static_cast<uint32_t>(new_value)});
}

LIBCXXW_NAMESPACE_END
