#ifndef connection_thread_h
#define connection_thread_h

#include "connection.H"
#include <x/threadmsgdispatcher.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN connectionObj::implObj::threadObj
	: public threadmsgdispatcherObj {

	const ref<infoObj> info;

 public:

	threadObj(const ref<infoObj> &info);
	~threadObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
