#include "connection_info.H"

LIBCXXW_NAMESPACE_START

//! Internal helper class used during construction.

class LIBCXX_INTERNAL connectionObj::implObj::infoObj::connection_handle {
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

connectionObj::implObj::infoObj::infoObj(const std::experimental::string_view &display)
	: infoObj(connection_handle(std::string(display.begin(),
						display.end())))
{
}

connectionObj::implObj::infoObj::infoObj(connection_handle &&handle)
	: conn(handle.conn), default_screen(handle.default_screen)
{
}

connectionObj::implObj::infoObj::~infoObj() noexcept
{
	xcb_disconnect(conn);
}

LIBCXXW_NAMESPACE_END
