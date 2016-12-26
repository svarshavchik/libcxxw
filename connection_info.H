#ifndef connection_info_h
#define connection_info_h

#include "connection.H"
#include <x/threadmsgdispatcher.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN connectionObj::implObj::infoObj : virtual public obj {
 public:

	class connection_handle;

	xcb_connection_t * const conn;

	const int default_screen;

	infoObj(const std::experimental::string_view &display);

	infoObj(connection_handle &&handle);

	~infoObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
