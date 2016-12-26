#include "connection_info.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

connectionObj::implObj::threadObj::threadObj(const ref<infoObj> &info)
	: info(info)
{
}

connectionObj::implObj::threadObj::~threadObj() noexcept=default;

LIBCXXW_NAMESPACE_END
