/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "connection_info.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

//! Internal helper class used during construction.

class LIBCXX_INTERNAL connection_infoObj::connection_handle {
 public:
	xcb_connection_t * conn;

	int default_screen;

	connection_handle(const std::string &display)
	{
		conn=xcb_connect(display.size() == 0 ?
				 nullptr:display.c_str(), &default_screen);
	}

	~connection_handle() noexcept=default;
};

connection_infoObj::connection_infoObj(const std::experimental::string_view &display)
	: connection_infoObj(connection_handle(std::string(display.begin(),
							   display.end())))
{
}

connection_infoObj::connection_infoObj(connection_handle &&handle)
	: conn(handle.conn), default_screen(handle.default_screen)
{
}

connection_infoObj::~connection_infoObj() noexcept
{
	xcb_disconnect(conn);
}

uint32_t connection_infoObj::alloc_xid()
{
	{
		available_xids_t::lock lock(available_xids);

		if (!lock->empty())
		{
			uint32_t xid=lock->front();
			lock->pop();
			return xid;
		}
	}

	uint32_t id=xcb_generate_id(conn);

	if (id == (uint32_t)-1)
		throw EXCEPTION("xcb_generate_id() failed");

	return id;
}

void connection_infoObj::release_xid(uint32_t xid) noexcept
{
	available_xids_t::lock lock(available_xids);

	lock->push(xid);
}

LIBCXXW_NAMESPACE_END
